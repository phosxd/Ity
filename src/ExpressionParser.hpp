#pragma once

#include <string>
#include <unordered_map>
#include <numeric>

#include "Common.hpp"
#include "ScriptErrors.hpp"
#include "ScopeState.hpp"

#include "Op/Add.hpp"


const std::string ALPHA = "AaBbCcDdEeFfGgHhIiJjKkLlMmOoPpQqRrSsTtUuVvWwXxYyZz0123456789";
const std::string NUM = "0123456789";
const std::string STRING_SYMBOLS = "'\"";
const std::string MISC_RESERVED_SYMBOLS = "_." + STRING_SYMBOLS;
std::vector<std::string> NORMAL_TYPES = {"OP","BOOL","INT","FLOAT","STR"};
std::unordered_map<std::string, Operation> OPERATIONS = {
	{"+", OP_Add},
};

std::unordered_map<std::string, std::vector<Variant>> expr_cache;


bool is_valid_name(std::string name) {
	unsigned int name_len = name.size();
	for(unsigned int i = 0; i < name_len; i++) {
		if ((ALPHA.find(name[i]) == std::string::npos) && name[i] != '_') {return false;}
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


bool check_ahead(std::string text, unsigned int start_idx, std::string substr) {
	unsigned int substr_len = substr.size();
	if (text.size() > start_idx+substr_len) {return false;}
	for (unsigned int i = 0; i < substr_len; i++) {
		if (text[start_idx+i] != substr[i]) {return false;}
	}
	return true;
}


VariantData get_literal_from_str(std::string type, std::string str_val) {
	if (type == "OP" || type == "REF" || type == "STR") {return str_val;}
	else if (type == "BOOL") {return (str_val == "true");}
	else if (type == "INT") {return std::stoi(str_val);}
	else if (type == "FLOAT") {return std::stof(str_val);}
	else {
		emit_error("Unexpected (ExpressionParser.get_literal_from_str): unknown type \"" + type + "\".");
		return 0;
	}
}


std::vector<Variant> expr_tokenize(std::string expr) {
	if (expr_cache.find(expr) != expr_cache.end()) {return expr_cache[expr];}
	unsigned int expr_len = expr.size();

	std::vector<Variant> sequence;
	Variant item = Variant{};
	std::string buffer;
	bool is_start = true;
	bool is_operator = false;
	bool is_string = false;
	char string_type;

	for (unsigned int i = 0; i < expr_len; i++) {
		if (is_string) {
			if (expr[i] == string_type) {
				is_string = false;
				continue;
			}
		}

		else {
			if (expr[i] == ' ' || expr[i] == '\n' || expr[i] == '\t') {continue;}

			if (is_start) {
				if (NUM.find(expr[i]) != std::string::npos) {
					item.t = "INT";
				}
				else if (STRING_SYMBOLS.find(expr[i]) != std::string::npos) {
					item.t = "STR";
					string_type = expr[i];
					is_string = true;
					is_start = false;
					continue;
				}
				else if (check_ahead(expr, i, "true") || check_ahead(expr, i, "false")) {
					item.t = "BOOL";
				}
				else if (is_valid_name(std::to_string(expr[i]))) {
					item.t = "REF";
				}
				is_start = false;
			}

			if (is_operator == false) {
				// If "." found in INT, convert to FLOAT.
				if (item.t == "INT") {
					if (expr[i] == '.') {
						item.t = "FLOAT";
					}
					else if ((NUM.find(expr[i]) == std::string::npos) && expr[i] != '_') {
						emit_error("Invalid number construct, invalid character \"" + std::to_string(expr[i]) + "\".");
						return sequence;
					}
				}

				// Start operator.
				if (is_special_symbol(expr[i])) {
					item.d = get_literal_from_str(item.t, buffer);
					sequence.push_back(item);
					item = Variant{};
					buffer = "";
					is_operator = true;
				}
			}

			// End operator.
			if (is_operator == true && expr_len > i+1 && not is_special_symbol(expr[i+1])) {
				sequence.push_back(Variant{
					"OP",
					buffer+expr[i],
				});
				item = Variant{};
				buffer = "";
				is_operator = false;
				is_start = true;
				continue;
			}
		}

		buffer += expr[i];
	}

	if (item.t != "." && buffer.size() > 0) {
		item.d = get_literal_from_str(item.t, buffer);
		buffer = "";
		sequence.push_back(item);
	}
	expr_cache[expr] = sequence;
	return sequence;
}


Variant resolve_variant(ScopeState& state, Variant& item) {
	if (item.t == "REF") {
		std::string name = std::get<std::string>(item.d);
		if (is_name_globally_free(state, name) == true) {
			emit_error(err_name_does_not_exist(name));
			return item;
		}
		return get_data_globally(state, name);
	}
	else if (exists_in_strvec(NORMAL_TYPES, item.t) == true) {
		return item;
	}
	emit_error("Unexpected (ExpressionParser.resolve_variant): unknown type. Please report bug.");
	return item;
}


Variant expr_exec(ScopeState& state, std::string expr) {
	std::vector<Variant> sequence = expr_tokenize(expr);
	//std::cout << "EXPR SEQ: " << sequence << "\n";
	const unsigned int seq_len = sequence.size();
	Variant result;
	Operation* op = nullptr;
	std::string op_symbol;
	for (unsigned int i = 0; i < seq_len; i++) {
		Variant item = sequence[i];
		if (op != nullptr) {
			Variant resolved_var = resolve_variant(state,item);
			result = op->exec(*op, state, result, resolved_var, op_symbol);
			op = nullptr;
			op_symbol = "";
			continue;
		}
		else if (item.t == "OP") {
			op_symbol = std::get<std::string>(item.d);
			if (OPERATIONS.find(op_symbol) == OPERATIONS.end()) {
				emit_error("Invalid operator \"" + op_symbol + "\".");
				return result;
			}
			op = &OPERATIONS.at(op_symbol);
			continue;
		}
		else {
			result = resolve_variant(state, item);
			continue;
		}
	}

	return result;
}
