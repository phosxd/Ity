#pragma once


constexpr unsigned int MAX_TEMPORARY_POOL_RESERVE = 16;
// This is a pool of all temporaries created in an expression.
// Systems have exactly until the next expr exec call to use the data of a temporary, before it gets deleted.
std::vector<Variant> temporary_pool;
std::vector<std::vector<Variant>> temporary_pool_stack;



// Call a native function.
inline Variant call_native_function(ScopeState& state, const NativeFunc_t& func, const ARR_t& args) {
	return func(state, args);
}


// Call a script function.
Variant call_script_function(ScopeState& state, const MAP_t& func, Variant& args) {
	// Throw error if maximum execution depth is reached.
	if (execution_depth > execution_depth_max) {
		emit_error(ERR_max_execution_depth, {std::to_string(execution_depth_max)});
		return VariantPresets.none;
	}

	const INT_t& func_token_index = AnyCast(INT_t,func.at("__i").d);
	const VariantType& func_return_type = static_cast<VariantType>(AnyCast(INT_t,func.at("__rt").d));
	const InstToken& func_token = InstTokenSeq[func_token_index];

	// Throw error if token is not a function token.
	// The only time this should happen is if the tokens are corrupted in some way.
	if (func_token.args[0] != "func") {
		emit_error(ERR_unexpected, {"FunctionHandler", "Missing function: " + std::to_string(func_token_index) + " isn't a \"fn\" token."});
		return VariantPresets.none;
	}

	//call_trace.push_back(current_line); call_trace.push_back(current_column);
	push_back_ongoing_scopes(); // Save ongoing scopes for later.
	temporary_pool_stack.push_back(std::move(temporary_pool));
	temporary_pool = {};

	// Create an alternate scope, for use inside the function.
	ScopeState func_state = create_new_scope_state(
		(MAP_t){
			{"__AG__",  std::move(args)},
			{"__RT__",  Variant{func_return_type, std::monostate(), VariantMode_dynamic_type}}, // Initialize return variable.
		},
		get_state_at_depth(state, AnyCast(INT_t,func.at("__si").d)) // Use function definition scope as the parent.
	);

	#ifdef RUNTIME_DEBUG
	if (debug_flags.scoping) std::cout << ANSI::orange << "New Alt Scope From: " << func_token.args[2] << "\n" << ANSI::reset;
	#endif

	Ity::exec(func_state, InstTokenSeq, func_token.i+1, AnyCast(unsigned int,func_token.meta[0])); // Execute the tokens in the function.
	restore_ongoing_scopes(); // Restore previously ongoing scopes, now that we are out of the function.
	temporary_pool = std::move(temporary_pool_stack.back());
	temporary_pool_stack.pop_back();


	// Get result & check if return type matches.
	const Variant& func_result = func_state.d["__RT__"];
	if (func_result.t != func_return_type && func_return_type != ANY) emit_error(ERR_return_type_mismatch, {get_variant_type_name(func_result.t), get_variant_type_name(func_return_type)});
	// Return result.
	//call_trace.pop_back(); call_trace.pop_back();

	#ifdef RUNTIME_DEBUG
	if (debug_flags.scoping) std::cout << ANSI::orange << "Destroyed Alt Scope From: " << func_token.args[2] << " \n" << ANSI::reset;
	#endif

	args = std::move(func_state.d["__AG__"]);
	return func_result;
}



inline void LN_COL_COUNTER(const char& ch, unsigned int& ln, unsigned int& col) {
	if (ch == '\t') col += tab_col_value; // IDEs want to be funny & have a tab count as multiple columns, so allow the user to set the tab value depending on their IDE settings.
	else if (ch == '\n') {ln++; col = 0;}
	else col++;
}


#define resovlve_potential_ref(state, var) (var->t == REF) ? get_data_globally(state, AnyCast(STR_t,var->d), &none_var) : ((var->t == PTR) ? AnyCast(Variant*,var->d) : var);




#include "Op/Arith.hpp"
#include "Op/Set.hpp"
#include "Op/Compare.hpp"
#include "Op/Access.hpp"
#include "Op/TypeCast.hpp"


constexpr std::string STRING_SYMBOLS = "'\""; // String identifier symbols.
constexpr std::string MISC_RESERVED_SYMBOLS = "_.,()[]{}@~" + STRING_SYMBOLS; // Symbols reserved for special functionality. Operation symbols should not contain any of these characters.
const std::unordered_map<std::string, const Operation*> OPERATIONS = {
	{"+",  &OP_Arith},
	{"-",  &OP_Arith},
	{"*",  &OP_Arith},
	{"/",  &OP_Arith},
	{"%",  &OP_Arith},

	{"=",   &OP_Set},
	{"+=",  &OP_Set},
	{"-=",  &OP_Set},
	{"*=",  &OP_Set},
	{"/=",  &OP_Set},
	{"%=",  &OP_Set},
	{"<<=", &OP_Set},

	{"==",  &OP_Compare},
	{"!=",  &OP_Compare},
	{">",   &OP_Compare},
	{">=",  &OP_Compare},
	{"<",   &OP_Compare},
	{"<=",  &OP_Compare},
	{"&&",  &OP_Compare},
	{"||",  &OP_Compare},

	{":",   &OP_Access},
	{"->",  &OP_TypeCast}
};

std::unordered_map<std::string, std::vector<ExprToken>> expr_cache;


const bool is_valid_name(const std::string& name) {
	const size_t& name_len = name.size();
	if (name_len == 0) return false;
	for (size_t i = 0; i < name_len; i++) {
		const bool is_digit = (NUM.find(name[i]) != std::string::npos);
		if (i == 0 && is_digit) return false;
		if ((ALPHA.find(name[i]) == std::string::npos && not is_digit) && name[i] != '_') return false;
	}
	return true;
}


const bool is_special_symbol(const char& ch) {
	return (
		ALPHA.find(ch) == std::string::npos
		&& NUM.find(ch) == std::string::npos
		&& MISC_RESERVED_SYMBOLS.find(ch) == std::string::npos
	);
}


const bool check_ahead(const std::string& text, const unsigned int& start_idx, const std::string& substr) {
	const size_t& substr_len = substr.size();
	if (text.size() < start_idx+substr_len) return false;
	for (size_t i = 0; i < substr_len; i++) {
		if (text.at(start_idx+i) != substr.at(i)) return false;
	}
	return true;
}


// Add the pending literal token if available.
// Also resets the current buffer.
void clean_up_buffer(ExprToken& result_token, ExprToken& item, std::string& buffer) {
	if (item.var.t != PLACEHOLDER) {
		item.var.d = get_literal_from_str(item.var.t, buffer);
		result_token.seq.push_back(item);
		buffer.clear();
	}
}


unsigned int final_char_count = 0;
// Tokenize an expression. Returns an ExprToken with type "ExprTokenType_sequence".
ExprToken expr_tokenize(const std::string& expr, const unsigned int ln=0, const unsigned int col=0) {
	ExprToken result_token = ExprToken{ln, col};
	result_token.t = ExprTokenType_sequence;

	// Return cached token if available.
	if (const auto& it = expr_cache.find(expr); it != expr_cache.end()) {
		result_token.seq = it->second;
		return result_token;
	}

	const size_t& expr_len = expr.size();
	std::string buffer; buffer.reserve(expr_len);
	std::string secondary_buffer;
	ExprToken item = {0,0, ExprTokenType_variant, Variant{PLACEHOLDER}};
	unsigned int ln_offset = 0;
	unsigned int col_offset = 0;
	unsigned int skip_chars = 0;

	bool is_start = true;
	bool is_operator = false;
	bool is_string = false;
	bool is_escaped_char = false;
	bool is_array = false;
	bool is_map = false;
	bool next_ref_is_str = false;

	size_t idx = 0;
	for (size_t i = 0; i < expr_len; i++) { idx = i;
		const char& ch = expr[i];
		LN_COL_COUNTER(ch,ln_offset,col_offset);

		// Skip chars.
		if (skip_chars > 0) {
			skip_chars -= 1;
			continue;
		}

		// Handle string logic.
		if (is_string) {
			// End escaped.
			if (is_escaped_char) {
				is_escaped_char = false;
				if (ch == 'n') buffer.push_back('\n');       // New line character.
				else if (ch == 't') buffer.push_back('\t');  // Tab character.
				else if (ch == 'e') buffer.push_back('\e');  // Escape character.
				// Standard character.
				else buffer.push_back(ch);
				continue;
			}
			// Start escaped.
			if (ch == '\\') {
				is_escaped_char = true;
				continue;
			}
			// End string.
			if (std::string(1,ch) == secondary_buffer) {
				is_string = false;
				continue;
			}
		}

		// Compile array of expressions into a Variant.
		else if (is_array || is_map) {
			const std::string& subexpr = expr.substr(i);
			if (not subexpr.empty()) {
				// Tokenize sub-expression & add to sequence.
				item.seq = expr_tokenize(subexpr, ln_offset, col_offset).seq;
				result_token.seq.push_back(item);
				// Throw error if operator token found.
				for (const ExprToken& subtoken : item.seq) {
					if (subtoken.var.t == OP) {
						emit_error(ERR_operators_not_allowed);
						return result_token;
					}
				}
				// Skip over characters inside the sub-expression.
				skip_chars = final_char_count;
			}
			if (is_array) is_array = false;
			if (is_map) is_map = false;
			item = ExprToken{ln_offset, col_offset, ExprTokenType_variant, {PLACEHOLDER}};
			continue;
		}

		else {
			// Ignore spaces.
			if (ch == ' ' || ch == '\n' || ch == '\t') continue;

			// Start sub-expression.
			if (ch == '(') {
				if (expr_len <= i+1) {
					emit_error(ERR_unexpected_char_at_expr_end, {"("});
					return result_token;
				}
				const std::string& subexpr = expr.substr(i+1);
				clean_up_buffer(result_token, item, buffer);
				// Create expression sequence token.
				item = ExprToken{
					ln_offset, col_offset+1,
					ExprTokenType_sequence, VariantPresets.empty,
					expr_tokenize(subexpr, ln_offset, col_offset+1).seq
				};
				// Add to sequence.
				result_token.seq.push_back(item);
				// Skip over characters inside the sub-expression.
				skip_chars = final_char_count+1;
				continue;
			}

			// End expression.
			if (ch == ')' || ch == ']' || ch == '}') {
				break;
			}

			if (is_start) {
				const bool next_ref_is_str_ = next_ref_is_str;
				next_ref_is_str = false;
				item = ExprToken{ln_offset, col_offset, ExprTokenType_variant, {PLACEHOLDER}};
				// Set type integer.
				if ((NUM.find(ch) != std::string::npos) || (ch == '-' && (expr_len > i && NUM.find(expr[i+1]) != std::string::npos ))) {
					item.var.t = INT;
					buffer += ch;
					is_start = false;
					continue;
				}
				// Start string.
				else if (STRING_SYMBOLS.find(ch) != std::string::npos) {
					item.var.t = STR;
					secondary_buffer = ch;
					is_string = true;
					is_start = false;
					continue;
				}
				// Set type bool.
				else if (check_ahead(expr, i, "true")) {
					secondary_buffer = "true";
					item.var.t = BOOL;
				}
				else if (check_ahead(expr, i, "false")) {
					secondary_buffer = "false";
					item.var.t = BOOL;
				}
				// Set type none.
				else if (check_ahead(expr, i, "none")) {
					secondary_buffer = "none";
					item.var.t = NONE;
				}
				// Set type reference.
				else if (ch == '@' || ch == '~' || is_valid_name(std::string(1,ch))) {
					if (next_ref_is_str_) item.var.t = STR; // Set type as string but don't set `is_string` so it's not treated as a string.
					else item.var.t = TREF;
				}
				// Set type array.
				else if (ch == '[') {
					item.t = ExprTokenType_sequence;
					item.var.t = ARR;
					is_array = true;
					is_start = false;
					continue;
				}
				// Set type map.
				else if (ch == '{') {
					item.t = ExprTokenType_sequence;
					item.var.t = MAP;
					is_map = true;
					is_start = false;
					continue;
				}
				is_start = false;
			}

			if (is_operator == false) {
				// Separate expression.
				if (ch == ',') {
					clean_up_buffer(result_token, item, buffer);
					item.var.t = PLACEHOLDER;
					is_start = true;
					continue;
				}

				// Start operator.
				else if (is_special_symbol(ch) == true) {
					clean_up_buffer(result_token, item, buffer);
					item.var.t = PLACEHOLDER;
					is_operator = true;
				}

				// Skip over underscore in numbers.
				else if ((item.var.t == INT || item.var.t == FLOAT) && ch == '_') {
					continue;
				}

				else if (item.var.t == INT) {
					// If "." found in INT, convert to FLOAT.
					if (ch == '.') item.var.t = FLOAT;
					// Throw error if invalid character found.
					else if ((NUM.find(ch) == std::string::npos)) {
						emit_error(ERR_invalid_character_for_construct, {"number", std::string(1,ch)}, ln_offset, col_offset);
						return result_token;
					}
				}

				else if (item.var.t == BOOL || item.var.t == NONE) {
					// If no longer matches the bool or none token, switch to a reference.
					if ((buffer+ch).size() >= secondary_buffer.size() && (buffer+ch) != secondary_buffer) {
						item.var.t = TREF;
						secondary_buffer.clear();
					}
				}

				else if (item.var.t == TREF || (item.var.t == STR && not is_string)) {
					// Convert reference dot accessor to proper accessor.
					if (ch == '.') {
						result_token.seq.push_back(ExprToken{
							ln_offset, col_offset,
							ExprTokenType_variant,
							{
								(item.var.t == STR) ? STR : TREF,
								buffer,
							}
						});
						result_token.seq.push_back(ExprToken{
							ln_offset, col_offset,
							ExprTokenType_variant,
							{OP, (STR_t)":"},
						});
						buffer.clear();
						next_ref_is_str = true;
						is_start = true;
						continue;
					}
				}
			}

			// End operator.
			if (is_operator == true && expr_len > i+1 && (expr[i+1] == ' ' or not is_special_symbol(expr[i+1])) ) {
				result_token.seq.push_back(ExprToken{
					ln_offset, col_offset+1,
					ExprTokenType_variant,
					{OP, buffer+ch},
				});
				buffer.clear();
				is_operator = false;
				is_start = true;
				continue;
			}
		}

		buffer.push_back(ch);
	}

	clean_up_buffer(result_token, item, buffer);
	expr_cache[expr] = result_token.seq;
	final_char_count = idx;
	return result_token;
}




Variant* resolve_variant(ScopeState& state, Variant& item) {
	// If typed reference...
	if (item.t == TREF) {
		const STR_t& name = AnyCast(STR_t,item.d);
		const STR_t& real_name = trim_left(trim_left(name,'@'),'~');

		// Throw error if variable is undefined.
		if (is_name_globally_free(state, real_name)) {
			emit_error(ERR_name_does_not_exist, {real_name});
			return &item;
		}

		// Get variable.
		Variant* ptr = get_data_globally(state, real_name);
		// Create named ref.
		if (name[0] == '@') {temporary_pool.push_back(Variant{REF, real_name}); return &temporary_pool.back();}
		// Deref pointer or named reference.
		else if (name[0] == '~') {
			switch (ptr->t) {
				case REF: return get_data_globally(state, AnyCast(STR_t,ptr->d), &none_var);
				case PTR: return AnyCast(Variant*,ptr->d);
				default: {
					emit_error(ERR_cannot_dereference, {real_name});
					return &item;
				}
			}
		}
		// Return variable.
		return ptr;
	}

	// Otherwise, just return the Variant as-is.
	return &item;
}


// Execute a sequence of ExprTokens. `token` itself is an ExprToken which should contain a sequence in `ExprToken.seq`.
Variant* expr_exec_(ScopeState& state, ExprToken& token, const bool subexpr=false, const unsigned int& ln=0, const unsigned int& col=0) {
	// Output sequence in debug mode.
	#ifdef RUNTIME_DEBUG
	if (debug_flags.expr_seq && not subexpr) {
		std::cout << ANSI::purple << "ExprToken Sequence: " << ANSI::reset << token.seq << "\n";
	};
	#endif

	const unsigned int line_ = ln;
	const unsigned int col_ = col-1;
	current_line = line_;
	current_column = col_;


	// `std::vector` invalidates all references to items inside it when it reallocates, reserve an upper-limit to make sure we wont reallocate.
	if (temporary_pool.capacity() < MAX_TEMPORARY_POOL_RESERVE) temporary_pool.reserve(MAX_TEMPORARY_POOL_RESERVE);

	// Resolve array.
	if (token.var.t == ARR) {
		ARR_t array; array.reserve(token.seq.size());
		for (ExprToken& subtoken : token.seq) {
			if (subtoken.t == ExprTokenType_sequence) array.push_back(*expr_exec_(state, subtoken, true, line_,col_));
			else array.push_back(*resolve_variant(state, subtoken.var));
		}

		temporary_pool.push_back(Variant{ARR, std::move(array)}); return &temporary_pool.back();
	}

	// Resolve map.
	else if (token.var.t == MAP) {
		// Throw error if there are an odd number of elements.
		if (token.seq.size() % 2 != 0) {
			emit_error(ERR_invalid_syntax, {"Map literal expects key-value pairs. ( {'a', 1, 'b', 2} )"});
			return &none_var;
		}
		MAP_t map; map.reserve(token.seq.size()/2);
		bool is_key = true;
		STR_t key;
		for (ExprToken& subtoken : token.seq) {
			if (is_key) {
				// Get key.
				const Variant* var = nullptr;
				if (subtoken.t == ExprTokenType_sequence) var = expr_exec_(state, subtoken, true, line_,col_);
				else var = resolve_variant(state, subtoken.var);
				// Throw error if key is not a string.
				if (var->t != STR) {
					emit_error(ERR_invalid_syntax, {"Map key must be a string"});
					return &none_var;
				}
				// Apply key.
				key = AnyCast(STR_t,var->d);
				is_key = false;
			}
			else {
				// Apply value.
				if (subtoken.t == ExprTokenType_sequence) map[key] = *expr_exec_(state, subtoken, true, line_,col_);
				else map[key] = *resolve_variant(state, subtoken.var);
				is_key = true;
			}
		}

		temporary_pool.push_back(Variant{MAP, std::move(map)}); return &temporary_pool.back();
	}


	const size_t& seq_len = token.seq.size();
	Variant* result = nullptr;
	Variant* second = nullptr;
	// Pre initialized variables.
	Variant pre_exec_result = VariantPresets.empty;
	Variant op_result = VariantPresets.empty;

	const Operation* op = nullptr;
	std::string op_symbol;

	for (size_t i = 0; i < seq_len; i++) {
		ExprToken& item = token.seq[i];
		current_line = line_ + item.ln;
		current_column = col_ + item.col;

		// Execute operator.
		if (op) {
			// Throw error if there is no first operand.
			if (not result) {
				emit_error(ERR_missing_operand);
				return result;
			}
			// Run operator pre-exec.
			if (op->pre_exec) {
				// Skip evaluation of second Variant if pre_exec says so...
				bool eval_second_operand = true;
				pre_exec_result.t = PLACEHOLDER; // Reset the type for reuse.
				// Run pre-executor.
				op->pre_exec(state, result, op_symbol, eval_second_operand, pre_exec_result, result);
				if (not eval_second_operand) {
					if (pre_exec_result.t != PLACEHOLDER) {
						temporary_pool.push_back(pre_exec_result); result = &temporary_pool.back();
					}
					op = nullptr;
					continue;
				}
			}
			// Get our second variant to operate on.
			// If second is a sequence...
			if (item.t == ExprTokenType_sequence) second = expr_exec_(state, item, true, current_line, current_column);
			// If it's a normal var...
			else second = resolve_variant(state, item.var);
			// Throw error if second is an operator.
			if (second->t == OP) {
				emit_error(ERR_invalid_syntax, {"Operator cannot be used as operand"});
				return result;
			}

			op_result.t = PLACEHOLDER; // Reset the type for reuse.
			op->exec(state, result, second, op_symbol, op_result, result); // Passing the `result` variable so the operator can potentially overwrite it.
			// If we reveive a direct value, set the result to that.
			if (op_result.t != PLACEHOLDER) {
				temporary_pool.push_back(op_result); result = &temporary_pool.back();
			}

			op = nullptr;
			continue;
		}


		else if (item.t == ExprTokenType_variant) {
			// Get operator.
			if (item.var.t == OP) {
				op_symbol = AnyCast(STR_t,item.var.d);
				// Find the Operation object from the symbol.
				if (const auto& it = OPERATIONS.find(op_symbol); it != OPERATIONS.end()) {
					op = it->second;
					continue;
				}
				// Throw error if invalid operator.
				emit_error(ERR_invalid_op, {op_symbol});
				return result;
			}
			// Get variant.
			else result = resolve_variant(state, item.var);
		}

		// Get value from sub-sequence
		else if (item.t == ExprTokenType_sequence) {
			result = expr_exec_(state, item, true, current_line, current_column);
		}
	}

	if (not result) {
		temporary_pool.push_back(VariantPresets.none);
		return &temporary_pool.back();
	}

	// Output result in debug mode.
	#ifdef RUNTIME_DEBUG
	if (debug_flags.expr_result && not subexpr) {
		std::cout << ANSI::purple << "Expression Result: " << ANSI::reset << ((result) ? *result : VariantPresets.none) << "\n";
	};
	#endif

	return result;
}




Variant* expr_exec(ScopeState& state, ExprToken& token, const bool subexpr=false, const unsigned int ln=0, const unsigned int col=0) {
	temporary_pool.clear();
	Variant* result = expr_exec_(state, token, subexpr, ln,col);

	// Throw error if we go over the temporary variant limit.
	if (temporary_pool.size() >= MAX_TEMPORARY_POOL_RESERVE) emit_error(ERR_max_temporaries_in_use, {std::to_string(temporary_pool.size()), std::to_string(MAX_TEMPORARY_POOL_RESERVE)});

	return result;
}




// Tokenize then execute an expression.
Variant expr_run(ScopeState& state, const std::string& expr) {
	ExprToken tokens = expr_tokenize(expr, current_line, current_column);
	return *expr_exec(state, tokens);
}




// Inserts a tokenized expression with proper meta data into the given `token.expr`.
// Expression is assumed to start at `start_idx` in `token.args`.
std::vector<std::string> tokenize_expr_from_inst_args(InstToken& token, const unsigned int& start_idx) {
	std::vector<std::string> new_args; new_args.reserve(start_idx);
	std::string expr_string;
	unsigned int i = 0;
	unsigned int ln = 0;
	unsigned int col = 0;
	for (const std::string& arg : token.args) {
		if (i < start_idx) {
			for (const char& ch : arg) LN_COL_COUNTER(ch,ln,col);
			new_args.push_back(arg);
		}
		else expr_string += ' '+token.args[i];
		i++;
	}

	token.expr = expr_tokenize(expr_string, token.ln+ln, token.col+col);
	return new_args;
}
