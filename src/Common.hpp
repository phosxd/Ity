#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

#include "ScriptErrors.hpp"




// Misc display overloads.
// -----------------------

// std::monostate
std::ostream& operator<<(std::ostream& os, const std::monostate& s) {
	return os << "std::monostate";
}


// uint8_t
std::ostream& operator<<(std::ostream& os, const uint8_t& s) {
	return os << std::to_string(s); // Convert to string, otherwie displays as empty.
}


// vector
template<class T_operator_lshift_vector_s>
std::ostream& operator<<(std::ostream& os, const std::vector<T_operator_lshift_vector_s>& s) {
	os << '[';
	const unsigned int len = s.size();
	for (unsigned int i = 0; i < len; i++) {
		if (i != 0) {os << ", ";}
		os << s[i];
	}
	return os << ']';
}


// unordered_map
template<class T_operator_lshift_umap_key_s, class T_operator_lshift_umap_value_s>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<T_operator_lshift_umap_key_s, T_operator_lshift_umap_value_s>& s) {
	os << '{';
	const unsigned int len = s.size();
	unsigned int idx = 0;
	for (auto i:s) {
		if (idx != 0) {os << ", ";}
		os << i.first << '=' << i.second;
		idx++;
	}
	return os << '}';
}




// VariantType.
// ------------


enum VariantType {
	// Meta types.
	INFERRED,
	ANY,
	OP,
	REF,
	// Real types.
	NONE,
	BOOL,
	INT,
	FLOAT,
	STR,
	ARR,
	MAP,
};


// Get string representation of a VariantType.
std::string get_variant_type_name(VariantType type) {
	switch (type) {
		// Meta types.
		case INFERRED: return "*";
		case ANY: return "ANY";
		case OP: return "OP";
		case REF: return "REF";
		// Real types.
		case BOOL: return "BOOL";
		case INT: return "INT";
		case FLOAT: return "FLOAT";
		case STR: return "STR";
		case ARR: return "ARR";
		case MAP: return "MAP";
	}
	return "NONE";
}


// Get VariantType from a string representation.
VariantType get_variant_type_from_name(std::string name) {
	// Meta types.
	if (name == "*") { return INFERRED;}
	else if (name == "ANY") {return ANY;}
	else if (name == "OP") {return OP;}
	else if (name == "REF") {return REF;}
	// Real types.
	else if (name == "BOOL") {return BOOL;}
	else if (name == "INT") {return INT;}
	else if (name == "FLOAT") {return FLOAT;}
	else if (name == "STR") {return STR;}
	else if (name == "ARR") {return ARR;}
	else if (name == "MAP") {return MAP;}
	return NONE;
}


std::ostream& operator<<(std::ostream& os, const VariantType& s) {
	return os << get_variant_type_name(s);
}




// VariantData.
// ------------

struct VariantDataStr {
	std::string v;
};


using VariantData = std::variant<
	std::monostate,
	bool,
	int,
	float,
	std::string,
	// Value is stored as an expression string which will need to be processed to get the actual value.
	VariantDataStr
>;


// Resolve VariantData to a real VariantType.
VariantType get_variant_data_type(const VariantData& d) {
	if (std::holds_alternative<bool>(d)) {return BOOL;}
	else if (std::holds_alternative<int>(d)) {return INT;}
	else if (std::holds_alternative<float>(d)) {return FLOAT;}
	else if (std::holds_alternative<std::string>(d)) {return STR;}
	// else if (std::holds_alternative<VariantDataStr>(d)) {
	// 	std::string val = std::get<VariantDataStr>(d).v;
	// 	return get_variant_type_from_name()
	// }
	return NONE;
}


std::ostream& operator<<(std::ostream& os, const VariantData& s) {
	if (std::holds_alternative<std::monostate>(s)) {
		os << std::get<std::monostate>(s);
	}
	else if (std::holds_alternative<bool>(s)) {
		os << std::get<bool>(s);
	}
	else if (std::holds_alternative<int>(s)) {
		os << std::get<int>(s);
	}
	else if (std::holds_alternative<float>(s)) {
		os << std::to_string(std::get<float>(s)); // `std::cout` wont show the full precision by default, so we convert to string.
	}
	else if (std::holds_alternative<std::string>(s)) {
		os << std::get<std::string>(s);
	}
	else if (std::holds_alternative<VariantDataStr>(s)) {
		os << std::get<VariantDataStr>(s).v;
	}
	return os;
}



VariantData operator+(const VariantData& a, const VariantData& b) {
	// If a is string & b is string...
	if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b)) {
		return std::get<std::string>(a) + std::get<std::string>(b);
	}
	// If a is int...
	else if (std::holds_alternative<int>(a)) {
		// If b is int...
		if (std::holds_alternative<int>(b)) {return std::get<int>(a) + std::get<int>(b);}
		// If b is float...
		else if (std::holds_alternative<float>(b)) {return std::get<int>(a) + std::get<float>(b);}
	}
	// If a is float...
	else if (std::holds_alternative<float>(a)) {;
		// If b is int...
		if (std::holds_alternative<int>(b)) {return std::get<float>(a) + std::get<int>(b);}
		// If b is float...
		else if (std::holds_alternative<float>(b)) {return std::get<float>(a) + std::get<float>(b);}
	}

	// Throw error is none matched.
	emit_error(err_operand_type_mismatch("add", get_variant_type_name(get_variant_data_type(a)), get_variant_type_name(get_variant_data_type(b))));
	return a;
}


VariantData operator-(const VariantData& a, const VariantData& b) {
	// If a is int...
	if (std::holds_alternative<int>(a)) {
		// If b is int...
		if (std::holds_alternative<int>(b)) {return std::get<int>(a) - std::get<int>(b);}
		// If b is float...
		else if (std::holds_alternative<float>(b)) {return std::get<int>(a) - std::get<float>(b);}
	}
	// If a is float...
	else if (std::holds_alternative<float>(a)) {;
		// If b is int...
		if (std::holds_alternative<int>(b)) {return std::get<float>(a) - std::get<int>(b);}
		// If b is float...
		else if (std::holds_alternative<float>(b)) {return std::get<float>(a) - std::get<float>(b);}
	}

	// Throw error is none matched.
	emit_error(err_operand_type_mismatch("subtract", get_variant_type_name(get_variant_data_type(a)), get_variant_type_name(get_variant_data_type(b))));
	return a;
}


VariantData operator*(const VariantData& a, const VariantData& b) {
	// If a is string & b is int.
	if (std::holds_alternative<std::string>(a) && std::holds_alternative<int>(b)) {
		int b_val = std::get<int>(b);
		if (b_val < 0) {
			emit_error("Cannot multiply string by a negative number.");
			return a;
		}
		std::string a_val = std::get<std::string>(a);
		std::string sum; sum.reserve(a_val.size()*b_val);
		for (unsigned int i = 0; i < b_val; i++) {sum += a_val;}
		return sum;
	}
	// If a is int...
	else if (std::holds_alternative<int>(a)) {
		// If b is int...
		if (std::holds_alternative<int>(b)) {return std::get<int>(a) * std::get<int>(b);}
		// If b is float...
		else if (std::holds_alternative<float>(b)) {return std::get<int>(a) * std::get<float>(b);}
	}
	// If a is float...
	else if (std::holds_alternative<float>(a)) {;
		// If b is int...
		if (std::holds_alternative<int>(b)) {return std::get<float>(a) * std::get<int>(b);}
		// If b is float...
		else if (std::holds_alternative<float>(b)) {return std::get<float>(a) * std::get<float>(b);}
	}

	// Throw error is none matched.
	emit_error(err_operand_type_mismatch("multiply", get_variant_type_name(get_variant_data_type(a)), get_variant_type_name(get_variant_data_type(b))));
	return a;
}


VariantData operator/(const VariantData& a, const VariantData& b) {
	// If a is int...
	if (std::holds_alternative<int>(a)) {
		// If b is int...
		if (std::holds_alternative<int>(b)) {return std::get<int>(a) / std::get<int>(b);}
		// If b is float...
		else if (std::holds_alternative<float>(b)) {return std::get<int>(a) / std::get<float>(b);}
	}
	// If a is float...
	else if (std::holds_alternative<float>(a)) {;
		// If b is int...
		if (std::holds_alternative<int>(b)) {return std::get<float>(a) / std::get<int>(b);}
		// If b is float...
		else if (std::holds_alternative<float>(b)) {return std::get<float>(a) / std::get<float>(b);}
	}

	// Throw error is none matched.
	emit_error(err_operand_type_mismatch("divide", get_variant_type_name(get_variant_data_type(a)), get_variant_type_name(get_variant_data_type(b))));
	return a;
}



// Variant.
// --------

struct Variant {
	VariantType t = NONE;
	VariantData d = std::monostate();
	uint8_t m = 0;
};


std::ostream& operator<<(std::ostream& os, const Variant& s) {
	return os << "{t=" << s.t << ", d=" << s.d << ", m=" << s.m << '}';
}



// InstToken.
// ----------


struct InstToken {
	unsigned int ln = 0;
	unsigned int col = 0;
	std::vector<std::string> args;
};


std::ostream& operator<<(std::ostream& os, const InstToken& s) {
	return os << "{ln=" << s.ln << ", col=" << s.col << ", args=" << s.args << '}';
}




// ExprToken.
// ----------


struct ExprToken {
	unsigned int ln = 0;
	unsigned int col = 0;
	Variant var;
};


std::ostream& operator<<(std::ostream& os, const ExprToken& s) {
	return os << "{ln=" << s.ln << ", col=" << s.col << ", var=" << s.var << '}';
}




// Scope State.
// ------------

struct ScopeState {
	ScopeState* p = nullptr;
	std::unordered_map<std::string,Variant> d;
};


std::ostream& operator<<(std::ostream& os, const ScopeState& s) {
	os << "ScopeState{p=" << s.p << ", d=" << s.d << "}";
	return os;
}




// Instruction.
// ------------

struct Instruction {
	unsigned int REQUIRED = 0;
	int OPTIONAL = 0;
	void (*exec)(Instruction& inst, ScopeState& state, std::vector<std::string> args, std::string symbol) = nullptr;
};


std::ostream& operator<<(std::ostream& os, const Instruction& s) {
	return os << "Instruction{REQUIRED=" << s.REQUIRED << ", OPTIONAL=" << s.OPTIONAL << "}";
}




// Operation.
// ----------

struct Operation {
	std::unordered_map<VariantType,std::vector<VariantType>> type_map;
	Variant (*exec)(Operation& op, ScopeState& state, Variant& first, Variant& second, std::string symbol) = nullptr;
};


std::ostream& operator<<(std::ostream& os, const Operation& s) {
	return os << "Operation{type_map=" << s.type_map << "}";
}




// Misc utility functions.
// -----------------------


// Returns the string with all instances of `ch` removed from the start of it.
std::string trim_left(std::string& text, char ch) {
	const unsigned int text_len = text.size();
	if (text_len == 0) {return text;}
	if (text.at(0) != ch) {return text;}
	bool ended = false;
	std::string result;
	result.reserve(text_len);
	for (unsigned int i = 0; i < text_len; i++) {
		if (text.at(i) != ch) {ended = true;}
		if (ended == true) {result.push_back(text.at(i));}
	}
	return result;
}


// Joins all elements in the vector into a new string, with each element separated by the given `sep`.
std::string join_str(std::vector<std::string>& vec, std::string sep) {
	const unsigned int vec_len = vec.size();
	std::string result = vec.front();
	result.reserve(sep.size() * (vec.size()-1));
	for (unsigned int i = 1; i < vec_len; i++) {
		result += sep + vec[i];
	}
	return result;
}


// Returns the number of strings that are empty inside the given vector.
unsigned int count_non_empty_strings(std::vector<std::string> items) {
	const unsigned int items_len = items.size();
	unsigned int count = 0;
	for (unsigned int i = 0; i < items_len; i++) {
		if (items[i].empty() == false) {
			count++;
		}
	}
	return count;
}


// Returns `false` if the string representation falls out of the 32-bit range for integers.
// NOTE: This is not an accurate check, it stops at 2,000,000,000 instead of the actual maximum.
bool is_int_str_32_in_range(std::string int_str) {
	int_str = trim_left(int_str, '0');
	if (int_str.size() == 0) {return true;}

	bool negative = (int_str.at(0) == '-');
	unsigned int digits = int_str.size();
	if (negative) {digits--;}
	// If too many digits, return false.
	if (digits > 10) {return false;}
	// If less than max, return true.
	else if (digits < 10) {return true;}

	uint8_t i = 0;
	if (negative) {i++;}
	if (int_str.at(i) == '1') {return true;}
	return false;
}


template<class T_exists_in_vec_v, class T_exists_in_vec_val>
bool exists_in_vec(T_exists_in_vec_v& v, T_exists_in_vec_val& val) {
	for (auto i:v) {
		if (i == val) {return true;}
	}
	return false;
}


// Performs checks on the Operation execution. If a check fails, it will throw an error.
// Checks:
//  - Not operating on type NONE.
//  - Variant types match `op.type_map`.
void CheckOperationExec(Operation& op, ScopeState& state, Variant& first, Variant& second, std::string op_str) {
	if (first.t == NONE || second.t == NONE) {
		emit_error(err_operand_type_mismatch(op_str, get_variant_type_name(first.t), ""));
		return;
	}

	if (op.type_map.find(first.t) == op.type_map.end()) {
		emit_error(err_operand_type_mismatch(op_str, get_variant_type_name(first.t), ""));
		return;
	}

	if (exists_in_vec(op.type_map.at(first.t), second.t) == false) {
		emit_error(err_operand_type_mismatch(op_str, get_variant_type_name(first.t), get_variant_type_name(second.t)));
		return;
	}
}
