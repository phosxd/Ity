#pragma once

#include <string>
#include <unordered_map>

#include "Common.hpp"
#include "ScriptErrors.hpp"
#include "ScopeState.hpp"

#include "Op/Add.hpp"
#include "Op/Subtract.hpp"
#include "Op/Multiply.hpp"
#include "Op/Divide.hpp"


const std::string ALPHA = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";
const std::string NUM = "0123456789";
const std::string STRING_SYMBOLS = "'\""; // String identifier symbols.
const std::string MISC_RESERVED_SYMBOLS = "_.()" + STRING_SYMBOLS; // Symbols reserved for special functionality. Operation symbols should not contain ay of these characters.
std::unordered_map<std::string, Operation> OPERATIONS = {
	{"+", OP_Add},
	{"+-", OP_Add},
	{"-", OP_Subtract},
	{"--", OP_Subtract},
	{"*", OP_Multiply},
	{"*-", OP_Multiply},
	{"/", OP_Divide},
	{"/-", OP_Divide},
};

std::unordered_map<std::string, std::vector<ExprToken>> expr_cache;


bool is_valid_name(std::string name) {
	unsigned int name_len = name.size();
	for(unsigned int i = 0; i < name_len; i++) {
		bool is_digit = (NUM.find(name[i]) != std::string::npos);
		if (i == 0 && is_digit) {return false;}
		if ((ALPHA.find(name[i]) == std::string::npos && is_digit == false) && name[i] != '_') {return false;}
	}
	return true;
}


bool is_special_symbol(char ch) {
	return (
		ALPHA.find(ch) == std::string::npos
		&& NUM.find(ch) == std::string::npos
		&& MISC_RESERVED_SYMBOLS.find(ch) == std::string::npos
	);
}


bool check_ahead(std::string& text, unsigned int start_idx, std::string substr, bool ignore_spaces=false) {
	unsigned int substr_len = substr.size();
	if (text.size() > start_idx+substr_len) {return false;}
	for (unsigned int i = 0; i < substr_len; i++) {
		if (ignore_spaces == true && text[start_idx+i] == ' ') {continue;}
		if (text[start_idx+i] != substr[i]) {return false;}
	}
	return true;
}


VariantData get_literal_from_str(VariantType type, std::string& str_val) {
	if (type == OP || type == REF || type == STR) {return str_val;}
	else if (type == BOOL) {return (str_val == "true");}
	else if (type == INT) {
		if (is_int_str_32_in_range(str_val) == false) {
			emit_error("Cannot initialize an integer larger than 1,999,999,999. You can go above this limit by adding numbers together, however they may wrap.");
			return std::monostate();
		}
		return std::stoi(str_val);
	}
	else if (type == FLOAT) {return std::stof(str_val);}
	else {return std::monostate();}
}


// Tokenize an expression. Returns an ExprToken with type "ExprTokenType_sequence".
ExprToken expr_tokenize(std::string expr, unsigned int ln=0, unsigned int col=0) {
	ExprToken result_token = ExprToken{current_line, current_column};
	result_token.t = ExprTokenType_sequence;

	// Return cached token with updated line & column (column is not entirely accurate).
	if (auto it = expr_cache.find(expr); it != expr_cache.end()) {
		const std::vector<ExprToken>& cached_seq = it->second;
		const unsigned int cached_seq_len = cached_seq.size();
		for (const ExprToken& cached_token: cached_seq) {
			ExprToken token = cached_token;
			token.ln = current_line;
			token.col = current_column;
			result_token.seq.push_back(token);
		}
		return result_token;
	}

	const unsigned int expr_len = expr.size();
	std::string buffer;
	std::string secondary_buffer;;
	buffer.reserve(expr_len);
	ExprToken item;
	if (ln == 0) {ln = current_line;}
	if (col == 0) {col = current_column;}
	unsigned int skip_to_ln = 0;
	unsigned int skip_to_col = 0;

	bool is_start = true;
	bool is_operator = false;
	bool is_string = false;
	bool is_escaped_char = false;

	for (unsigned int i = 0; i < expr_len; i++) {
		// Advance column or line number.
		col++;
		if(expr[i] == '\n') {
			ln++;
			col = 0;
		}

		// Skip to desired column or line number.
		if (skip_to_ln != 0) {
			if (ln >= skip_to_ln) {skip_to_ln = 0;}
			else {continue;}
		}
		if (skip_to_col != 0) {
			if (col >= skip_to_col) {skip_to_col = 0;}
			else {continue;}
		}

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
				if (subexpr.empty() == false) {
					if (item.var.t != NONE && buffer.size() > 0) {
						item.var.d = get_literal_from_str(item.var.t, buffer);
						result_token.seq.push_back(item);
						buffer = "";
					}

					// Tokenize sub-expression.
					ExprToken token = expr_tokenize(subexpr, ln, col);
					// Create expression sequence token.
					item = ExprToken{token.ln, token.col};
					item.t = ExprTokenType_sequence;
					item.seq = token.seq;
					// Add to sequence.
					result_token.seq.push_back(item);
					// Skip over characters inside the sub-expression.
					skip_to_ln = ln+token.ln-1;
					skip_to_col = col+token.col-1;

					buffer = "";
				}
				continue;
			}

			// End expression.
			if (expr[i] == ')') {
				break;
			}

			if (is_start) {
				item = ExprToken{ln,col};
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
				else if (is_valid_name(std::string(1,expr[i]))) {
					item.var.t = REF;
				}
				is_start = false;
			}

			if (is_operator == false) {
				// Start operator.
				if (is_special_symbol(expr[i]) == true) {
					item.var.d = get_literal_from_str(item.var.t, buffer);
					result_token.seq.push_back(item);
					item = ExprToken{ln,col};
					buffer = "";
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
						emit_error("Invalid number construct, invalid character \"" + std::string(1,expr[i]) + "\".", ln, col);
						return result_token;
					}
				}

				else if (item.var.t == BOOL || item.var.t == REF) {
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
					ln,col,
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

	if (item.var.t != NONE && buffer.size() > 0) {
		item.var.d = get_literal_from_str(item.var.t, buffer);
		buffer = "";
		result_token.seq.push_back(item);
	}
	expr_cache[expr] = result_token.seq;

	result_token.ln = ln;
	result_token.col = col;
	return result_token;
}


Variant resolve_variant(ScopeState& state, Variant& item) {
	if (item.t == REF) {
		std::string name = std::get<std::string>(item.d);
		if (is_name_globally_free(state, name) == true) {
			emit_error(err_name_does_not_exist(name));
			return item;
		}
		return get_data_globally(state, name);
	}

	return item;
}


// Execute a sequence of ExprTokens.
Variant expr_exec(ScopeState& state, std::vector<ExprToken>& sequence, bool subexpr=false) {
	// Output sequence in debug mode.
	if (debug_flags.expr_seq && not subexpr) {
		std::cout << ANSI.purple << "ExprToken Sequence: " << ANSI.reset << sequence << "\n";
	};

	const unsigned int seq_len = sequence.size();
	Variant result;
	Operation* op = nullptr;
	std::string op_symbol;
	for (unsigned int i = 0; i < seq_len; i++) {
		ExprToken item = sequence[i];
		current_line = item.ln;
		current_column = item.col;

		// Execute operator.
		if (op != nullptr) {
			Variant resolved_var;
			// Get variant.
			if (item.t == ExprTokenType_variant) {resolved_var = resolve_variant(state, item.var);}
			// Get variant from sub-expression.
			else if (item.t == ExprTokenType_sequence) {resolved_var = expr_exec(state, item.seq, true);}
			// Execute...
			result = op->exec(*op, state, result, resolved_var, op_symbol);
			op = nullptr;
			op_symbol = "";
			continue;
		}

		else if (item.t == ExprTokenType_variant) {
			// Get operator.
			if (item.var.t == OP) {
				op_symbol = std::get<std::string>(item.var.d);
				if (OPERATIONS.find(op_symbol) == OPERATIONS.end()) {
					emit_error("Invalid operator \"" + op_symbol + "\".");
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
			result = expr_exec(state, item.seq);
			continue;
		}
	}

	// Output result in debug mode.
	if (debug_flags.expr_result && not subexpr) {
		std::cout << ANSI.purple << "Expression result: " << ANSI.reset << result << "\n";
	};

	return result;
}



// Tokenize then execute an expression.
Variant expr_run(ScopeState& state, std::string expr) {
	ExprToken token = expr_tokenize(expr);
	return expr_exec(state, token.seq);
}
