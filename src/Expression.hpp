#pragma once

#include "Op/Arith.hpp"
#include "Op/Compare.hpp"


const std::string ALPHA = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";
const std::string NUM = "0123456789";
const std::string STRING_SYMBOLS = "'\""; // String identifier symbols.
const std::string MISC_RESERVED_SYMBOLS = "_.,()[]{}" + STRING_SYMBOLS; // Symbols reserved for special functionality. Operation symbols should not contain any of these characters.
std::unordered_map<std::string, Operation> OPERATIONS = {
	{"+", OP_Arith},
	{"-", OP_Arith},
	{"*", OP_Arith},
	{"/", OP_Arith},
	{"%", OP_Arith},
	{"==", OP_Compare},
	{"!=", OP_Compare},
	{">", OP_Compare},
	{">=", OP_Compare},
	{"<", OP_Compare},
	{"<=", OP_Compare},
};

std::unordered_map<std::string, std::vector<ExprToken>> expr_cache;


bool is_valid_name(const std::string& name) {
	unsigned int name_len = name.size();
	for(unsigned int i = 0; i < name_len; i++) {
		bool is_digit = (NUM.find(name[i]) != std::string::npos);
		if (i == 0 && is_digit) {return false;}
		if ((ALPHA.find(name[i]) == std::string::npos && is_digit == false) && name[i] != '_') {return false;}
	}
	return true;
}


bool is_special_symbol(const char& ch) {
	return (
		ALPHA.find(ch) == std::string::npos
		&& NUM.find(ch) == std::string::npos
		&& MISC_RESERVED_SYMBOLS.find(ch) == std::string::npos
	);
}


bool check_ahead(const std::string& text, const unsigned int start_idx, const std::string substr, const bool ignore_spaces=false) {
	unsigned int substr_len = substr.size();
	if (text.size() < start_idx+substr_len) {return false;}
	for (unsigned int i = 0; i < substr_len; i++) {
		if (ignore_spaces == true && text[start_idx+i] == ' ') {continue;}
		if (text[start_idx+i] != substr[i]) {return false;}
	}
	return true;
}


VariantData get_literal_from_str(const VariantType type, const std::string& str_val) {
	if (type == OP || type == REF || type == STR) {return str_val;}
	else if (type == BOOL) {return (str_val == "true");}
	else if (type == INT) {
		if (is_int_str_32_in_range(str_val) == false) {
			emit_error("Cannot initialize an integer larger than 1,999,999,999. You can go above this limit by adding numbers together, however they may wrap.");
			return std::any();
		}
		return std::stoi(str_val);
	}
	else if (type == FLOAT) {return std::stof(str_val);}
	else {return std::any();}
}


// ExprToken tokenize_array(const std::string arr_expr, unsigned int ln=0, unsigned int col=0) {
//
// }


// Add the pending literal token if available.
// Also resets the current buffer.
void clean_up_buffer(ExprToken& result_token, ExprToken& item, std::string& buffer) {
	if (item.var.t != NONE && buffer.size() > 0) {
		item.var.d = get_literal_from_str(item.var.t, buffer);
		result_token.seq.push_back(item);
		buffer = "";
	}
}


unsigned int final_ln_offset = 0;
unsigned int final_col_offset = 0;
// Tokenize an expression. Returns an ExprToken with type "ExprTokenType_sequence".
ExprToken expr_tokenize(const std::string expr, unsigned int ln=0, unsigned int col=0) {
	ExprToken result_token = ExprToken{ln, col};
	result_token.t = ExprTokenType_sequence;

	// Return cached token if available.
	if (auto it = expr_cache.find(expr); it != expr_cache.end()) {
		result_token.seq = it->second;
		return result_token;
	}

	const unsigned int expr_len = expr.size();
	std::string buffer;
	std::string secondary_buffer;;
	buffer.reserve(expr_len);
	ExprToken item;
	unsigned int ln_offset = 0;
	unsigned int col_offset = 0;
	unsigned int skip_to_ln = 0;
	unsigned int skip_to_col = 0;

	bool is_start = true;
	bool is_operator = false;
	bool is_string = false;
	bool is_escaped_char = false;
	bool is_array = false;

	for (unsigned int i = 0; i < expr_len; i++) {
		// Advance column or line number.
		col_offset++;
		if(expr[i] == '\n') {
			ln_offset++;
			col_offset = 0;
		}

		// Skip to desired column or line number.
		if (skip_to_ln != 0) {
			if (ln_offset >= skip_to_ln) {skip_to_ln = 0;}
			else {continue;}
		}
		if (skip_to_col != 0) {
			if (col_offset >= skip_to_col) {skip_to_col = 0;}
			else {continue;}
		}

		// Handle string logic.
		if (is_string) {
			// End escaped.
			if (is_escaped_char == true) {
				is_escaped_char = false;
				buffer.push_back(expr[i]);
				continue;
			}
			// Start escaped.
			if (expr[i] == '\\') {
				is_escaped_char = true;
				continue;
			}
			// End string.
			if (std::string(1,expr[i]) == secondary_buffer) {
				is_string = false;
				continue;
			}
		}

		// Compile array of expressions into a Variant.
		else if (is_array) {
			if (expr_len <= i+1) {
				emit_error("Unexpected \"[\" character at end of expression.");
				return result_token;
			}
			std::string subexpr = expr.substr(i);
			if (not subexpr.empty()) {
				clean_up_buffer(result_token, item, buffer);
				// Tokenize sub-expression.
				ExprToken token = expr_tokenize(subexpr, ln_offset, col_offset);
				// Set data.
				item.seq = token.seq;
				// Add to sequence.
				result_token.seq.push_back(item);
				// Skip over characters inside the sub-expression.
				skip_to_ln = ln_offset + final_ln_offset;
				skip_to_col = col_offset + (final_col_offset) + 1;
			}
			continue;
		}

		else {
			// Ignore spaces.
			if (expr[i] == ' ' || expr[i] == '\n' || expr[i] == '\t') {continue;}

			// Start sub-expression.
			if (expr[i] == '(') {
				if (expr_len <= i+1) {
					emit_error("Unexpected \"(\" character at end of expression.");
					return result_token;
				}
				std::string subexpr = expr.substr(i+1);
				if (not subexpr.empty()) {
					clean_up_buffer(result_token, item, buffer);
					// Tokenize sub-expression.
					ExprToken token = expr_tokenize(subexpr, ln_offset, col_offset);
					// Create expression sequence token.
					item = ExprToken{token.ln, token.col};
					item.t = ExprTokenType_sequence;
					item.seq = token.seq;
					// Add to sequence.
					result_token.seq.push_back(item);
					// Skip over characters inside the sub-expression.
					skip_to_ln = ln_offset + final_ln_offset;
					skip_to_col = col_offset + (final_col_offset) + 1;
				}
				continue;
			}

			// End expression.
			if (expr[i] == ')' || expr[i] == ']') {
				break;
			}

			if (is_start) {
				item = ExprToken{ln_offset, col_offset};
				// Set type integer.
				if (NUM.find(expr[i]) != std::string::npos) {
					item.var.t = INT;
				}
				// Start string.
				else if (STRING_SYMBOLS.find(expr[i]) != std::string::npos) {
					item.var.t = STR;
					secondary_buffer = expr[i];
					is_string = true;
					is_start = false;
					continue;
				}
				// Set type bool.
				else if (check_ahead(expr, i, "true", true)) {
					secondary_buffer = "true";
					item.var.t = BOOL;
				}
				else if (check_ahead(expr, i, "false", true)) {
					secondary_buffer = "false";
					item.var.t = BOOL;
				}
				// Set type none.
				else if (check_ahead(expr, i, "none", true)) {
					secondary_buffer = "none";
					item.var.t = NONE;
				}
				// Set type reference.
				else if (is_valid_name(std::string(1,expr[i]))) {
					item.var.t = REF;
				}
				// Set type array.
				else if (expr[i] == '[') {
					item.t = ExprTokenType_sequence;
					item.var.t = ARR;
					is_array = true;
					is_start = false;
					continue;
				}
				is_start = false;
			}

			if (is_operator == false) {
				// Separate expression.
				if (expr[i] == ',') {
					clean_up_buffer(result_token, item, buffer);
					item.var.t = NONE;
					is_start = true;
					continue;
				}

				// Start operator.
				else if (is_special_symbol(expr[i]) == true) {
					clean_up_buffer(result_token, item, buffer);
					item.var.t = NONE;
					is_operator = true;
				}

				// Skip over underscore in numbers.
				else if ((item.var.t == INT || item.var.t == FLOAT) && expr[i] == '_') {
					continue;
				}

				else if (item.var.t == INT) {
					// If "." found in INT, convert to FLOAT.
					if (expr[i] == '.') {
						item.var.t = FLOAT;
					}
					// Throw error if invalid character found.
					else if ((NUM.find(expr[i]) == std::string::npos)) {
						emit_error("Invalid character for number construct \"" + std::string(1,expr[i]) + "\".", ln, col);
						return result_token;
					}
				}

				else if (item.var.t == BOOL || item.var.t == NONE) {
					// If no longer matches the bool or none token, switch to a reference.
					if ((buffer+expr[i]).size() >= secondary_buffer.size() && (buffer+expr[i]) != secondary_buffer) {
						item.var.t = REF;
						secondary_buffer = "";
					}
				}
			}

			// End operator.
			if (is_operator == true && expr_len > i+1 && (expr[i+1] == ' ' or not is_special_symbol(expr[i+1])) ) {
				result_token.seq.push_back(ExprToken{
					ln_offset, col_offset,
					ExprTokenType_variant,
					{
						OP,
						buffer+expr[i],
					},
				});
				buffer = "";
				is_operator = false;
				is_start = true;
				continue;
			}
		}

		buffer.push_back(expr[i]);
	}

	clean_up_buffer(result_token, item, buffer);
	expr_cache[expr] = result_token.seq;
	final_ln_offset = ln_offset;
	final_col_offset = col_offset;
	return result_token;
}


Variant resolve_variant(ScopeState& state, const Variant& item) {
	if (item.t == REF) {
		std::string name = std::any_cast<std::string>(item.d);
		if (is_name_globally_free(state, name) == true) {
			emit_error(err_name_does_not_exist(name));
			return item;
		}
		return get_data_globally(state, name);
	}

	return item;
}


// Execute a sequence of ExprTokens. `token` itself is an ExprToken which should contain a sequence in `ExprToken.seq`.
Variant expr_exec(ScopeState& state, const ExprToken& token, const bool subexpr=false, unsigned int ln_offset=0, unsigned int col_offset=0) {
	// Output sequence in debug mode.
	if (debug_flags.expr_seq && not subexpr) {
		std::cout << ANSI::purple << "ExprToken Sequence: " << ANSI::reset << token.seq << "\n";
	};

	const unsigned int seq_len = token.seq.size();
	Variant result;
	Operation* op = nullptr;
	std::string op_symbol;
	for (unsigned int i = 0; i < seq_len; i++) {
		const ExprToken& item = token.seq[i];
		current_line = ln_offset + token.ln + item.ln;
		current_column = col_offset + token.col + (item.col-1);

		// Execute operator.
		if (op != nullptr) {
			Variant resolved_var;
			// Get variant.
			if (item.t == ExprTokenType_variant) {resolved_var = resolve_variant(state, item.var);}
			// Get variant from sub-expression.
			else if (item.t == ExprTokenType_sequence) {resolved_var = expr_exec(state, item, true, current_line, current_column);}
			// Execute...
			result = op->exec(*op, state, result, resolved_var, op_symbol);
			op = nullptr;
			op_symbol = "";
			continue;
		}

		else if (item.t == ExprTokenType_variant) {
			// Get operator.
			if (item.var.t == OP) {
				op_symbol = std::any_cast<std::string>(item.var.d);
				if (OPERATIONS.find(op_symbol) == OPERATIONS.end()) {
					emit_error(err_invalid_op(op_symbol));
					return result;
				}
				op = &OPERATIONS.at(op_symbol);
				continue;
			}
			// Get variant.
			else {
				result = resolve_variant(state, item.var);
				continue;
			}
		}

		// Get value from sub-sequence
		else if (item.t == ExprTokenType_sequence) {
			if (item.var.t == NONE) {
				result = expr_exec(state, item, current_line, current_column);
			}
			else if (item.var.t == ARR) {
				std::vector<Variant> array;
				array.reserve(item.seq.size());
				for (ExprToken token:item.seq) {
					if (token.var.t == OP) {
						emit_error("Operators not allowed inside \"ARR\". Use sub-expressions instead ( YES: [1, (1+1), 3]  |  NO: [1, 1+1, 3] ).");
						return result;
					}
					else if (token.t == ExprTokenType_sequence) {array.push_back(expr_exec(state, token, current_line, current_column));}
					else {array.push_back(resolve_variant(state, token.var));}
				}
				result = Variant{};
				result.t = ARR;
				result.d = array;
			}
		}
	}

	// Output result in debug mode.
	if (debug_flags.expr_result && not subexpr) {
		std::cout << ANSI::purple << "Expression result: " << ANSI::reset << result << "\n";
	};

	return result;
}



// Tokenize then execute an expression.
Variant expr_run(ScopeState& state, const std::string& expr) {
	const ExprToken& token = expr_tokenize(expr, current_line, current_column);
	return expr_exec(state, token);
}
