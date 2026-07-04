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


bool check_ahead(std::string& text, unsigned int start_idx, std::string substr) {
	unsigned int substr_len = substr.size();
	if (text.size() > start_idx+substr_len) {return false;}
	for (unsigned int i = 0; i < substr_len; i++) {
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
			return None{};
		}
		return std::stoi(str_val);
	}
	else if (type == FLOAT) {return std::stof(str_val);}
	else {return None{};}
}


std::vector<ExprToken> expr_tokenize(std::string expr) {
	std::vector<ExprToken> sequence;

	// Return cached token with updated line & column (column is not entirely accurate).
	if (auto it = expr_cache.find(expr); it != expr_cache.end()) {
		const std::vector<ExprToken>& cached_seq = it->second;
		const unsigned int cached_seq_len = cached_seq.size();
		for (const ExprToken& cached_token: cached_seq) {
			ExprToken token = cached_token;
			token.ln = current_line;
			token.col = current_column;
			sequence.push_back(token);
		}
		return sequence;
	}

	const unsigned int expr_len = expr.size();
	std::string buffer;
	buffer.reserve(expr_len);
	ExprToken item;
	unsigned int ln = current_line;
	unsigned int col = current_column;

	bool is_start = true;
	bool is_operator = false;
	bool is_string = false;
	char string_type;

	for (unsigned int i = 0; i < expr_len; i++) {
		// Advance column or line number.
		col++;
		if(expr[i] == '\n') {
			ln++;
			col = 0;
		}

		if (is_string) {
			if (expr[i] == string_type) {
				is_string = false;
				continue;
			}
		}

		else {
			if (expr[i] == ' ' || expr[i] == '\n' || expr[i] == '\t') {continue;}

			if (is_start) {
				item = ExprToken{ln,col};
				if (NUM.find(expr[i]) != std::string::npos) {
					item.var.t = INT;
				}
				else if (STRING_SYMBOLS.find(expr[i]) != std::string::npos) {
					item.var.t = STR;
					string_type = expr[i];
					is_string = true;
					is_start = false;
					continue;
				}
				else if (check_ahead(expr, i, "true") || check_ahead(expr, i, "false")) {
					item.var.t = BOOL;
				}
				else if (check_ahead(expr, i, "none")) {
					item.var.t = NONE;
				}
				else if (is_valid_name(std::string(1,expr[i]))) {
					item.var.t = REF;
				}
				is_start = false;
			}

			if (is_operator == false) {
				// Start operator.
				if (is_special_symbol(expr[i])) {
					item.var.d = get_literal_from_str(item.var.t, buffer);
					sequence.push_back(item);
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
						return sequence;
					}
				}
			}

			// End operator.
			if (is_operator == true && expr_len > i+1 && not is_special_symbol(expr[i+1])) {
				sequence.push_back(ExprToken{
					ln,col,
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
		sequence.push_back(item);
	}
	expr_cache[expr] = sequence;
	return sequence;
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


Variant expr_exec(ScopeState& state, std::string expr) {
	std::vector<ExprToken> sequence = expr_tokenize(expr);
	//std::cout << "EXPR SEQ: " << sequence << "\n";
	const unsigned int seq_len = sequence.size();
	Variant result;
	Operation* op = nullptr;
	std::string op_symbol;
	for (unsigned int i = 0; i < seq_len; i++) {
		ExprToken item = sequence[i];
		current_line = item.ln;
		current_column = item.col;

		if (op != nullptr) {
			Variant resolved_var = resolve_variant(state, item.var);
			result = op->exec(*op, state, result, resolved_var, op_symbol);
			op = nullptr;
			op_symbol = "";
			continue;
		}
		else if (item.var.t == OP) {
			op_symbol = std::get<std::string>(item.var.d);
			if (OPERATIONS.find(op_symbol) == OPERATIONS.end()) {
				emit_error("Invalid operator \"" + op_symbol + "\".");
				return result;
			}
			op = &OPERATIONS.at(op_symbol);
			continue;
		}
		else {
			result = resolve_variant(state, item.var);
			continue;
		}
	}

	return result;
}
