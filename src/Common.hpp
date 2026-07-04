#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <algorithm>

#include "ScriptErrors.hpp"


template<class D_um>
std::ostream& display_unordered_map(std::ostream& os, const D_um& s) {
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


template<class D_v>
std::ostream& display_vector(std::ostream& os, const D_v& s) {
	os << '[';
	const unsigned int len = s.size();
	for (unsigned int i = 0; i < len; i++) {
		if (i != 0) {os << ", ";}
		os << s[i];
	}
	return os << ']';
}




// Misc display overloads.
// -----------------------

// uint8_t
std::ostream& operator<<(std::ostream& os, const uint8_t& s) {
	return os << std::to_string(s); // Convert to string, otherwie displays as empty.
}
// vector<string>
std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& s) {
	return display_vector(os, s);
}




// VariantType.
// ------------


enum VariantType {
	ANY,
	NONE,
	OP,
	REF,
	BOOL,
	INT,
	FLOAT,
	STR,
};


// Get string representation of a VariantType.
std::string get_variant_type_name(VariantType type) {
	if (type == ANY) {return "ANY";}
	if (type == OP) {return "OP";}
	if (type == REF) {return "REF";}
	if (type == BOOL) {return "BOOL";}
	if (type == INT) {return "INT";}
	if (type == FLOAT) {return "FLOAT";}
	if (type == STR) {return "STR";}
	return "NONE";
}

// Get VariantType from a string representation.
VariantType get_variant_type_from_name(std::string name) {
	if (name == "ANY") {return ANY;}
	if (name == "OP") {return OP;}
	if (name == "REF") {return REF;}
	if (name == "BOOL") {return BOOL;}
	if (name == "INT") {return INT;}
	if (name == "FLOAT") {return FLOAT;}
	if (name == "STR") {return STR;}
	return NONE;
}


std::ostream& operator<<(std::ostream& os, const VariantType& s) {
	return os << get_variant_type_name(s);
}
// Vector.
std::ostream& operator<<(std::ostream& os, const std::vector<VariantType>& s) {
	return display_vector(os, s);
}




// VariantData.
// ------------


struct None{};
std::ostream& operator<<(std::ostream& os, const None& s) {
	return os << "None";
}


using VariantData = std::variant<None,bool,int,float,std::string>;


// Resolve VariantData to a real VariantType.
VariantType get_variant_data_type(VariantData& d) {
	if (std::holds_alternative<bool>(d)) {return BOOL;}
	else if (std::holds_alternative<int>(d)) {return INT;}
	else if (std::holds_alternative<float>(d)) {return FLOAT;}
	else if (std::holds_alternative<std::string>(d)) {return STR;}
	return NONE;
}


VariantData operator+(const VariantData& a, const VariantData& b) {
	// No addition on None.
	if (std::holds_alternative<None>(a) || std::holds_alternative<None>(b)) {
		emit_error(err_operand_type_mismatch("add", get_variant_type_name(NONE), ""));
	}
	// No addition on bool.
	else if (std::holds_alternative<bool>(a) || std::holds_alternative<bool>(b)) {
		emit_error(err_operand_type_mismatch("add", get_variant_type_name(BOOL), ""));
	}

	// If a is string & b is string...
	else if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b)) {
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
	emit_error("Unable to add variants.");
	return a;
}


VariantData operator-(const VariantData& a, const VariantData& b) {
	// No subtraction on None.
	if (std::holds_alternative<None>(a) || std::holds_alternative<None>(b)) {
		emit_error(err_operand_type_mismatch("subtract", get_variant_type_name(NONE), ""));
	}
	// No subtraction on bool.
	else if (std::holds_alternative<bool>(a) || std::holds_alternative<bool>(b)) {
		emit_error(err_operand_type_mismatch("subtract", get_variant_type_name(BOOL), ""));
	}
	// No subtraction on string.
	else if (std::holds_alternative<std::string>(a) || std::holds_alternative<std::string>(b)) {
		emit_error(err_operand_type_mismatch("subtract", get_variant_type_name(STR), ""));
	}

	// If a is int...
	else if (std::holds_alternative<int>(a)) {
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
	emit_error("Unable to subtract variants.");
	return a;
}


VariantData operator*(const VariantData& a, const VariantData& b) {
	// No multiplication on None.
	if (std::holds_alternative<None>(a) || std::holds_alternative<None>(b)) {
		emit_error(err_operand_type_mismatch("multiply", get_variant_type_name(NONE), ""));
	}
	// No multiplication on bool.
	else if (std::holds_alternative<bool>(a) || std::holds_alternative<bool>(b)) {
		emit_error(err_operand_type_mismatch("multiply", get_variant_type_name(BOOL), ""));
	}

	// If a is string & b is int.
	else if (std::holds_alternative<std::string>(a) && std::holds_alternative<int>(b)) {
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
	emit_error("Unable to multiply variants.");
	return a;
}


VariantData operator/(const VariantData& a, const VariantData& b) {
	// No division on None.
	if (std::holds_alternative<None>(a) || std::holds_alternative<None>(b)) {
		emit_error(err_operand_type_mismatch("divide", get_variant_type_name(NONE), ""));
	}
	// No division on bool.
	else if (std::holds_alternative<bool>(a) || std::holds_alternative<bool>(b)) {
		emit_error(err_operand_type_mismatch("divide", get_variant_type_name(BOOL), ""));
	}
	// No division on string.
	else if (std::holds_alternative<std::string>(a) || std::holds_alternative<std::string>(b)) {
		emit_error(err_operand_type_mismatch("divide", get_variant_type_name(STR), ""));
	}

	// If a is int...
	else if (std::holds_alternative<int>(a)) {
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
	emit_error("Unable to divide variants.");
	return a;
}


std::ostream& operator<<(std::ostream& os, const VariantData& s) {
	if (std::holds_alternative<None>(s)) {
		os << std::get<None>(s);
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
	return os;
}




// Variant.
// --------

struct Variant {
	VariantType t = NONE;
	VariantData d = None{};
	uint8_t m = 0;
};


std::ostream& operator<<(std::ostream& os, const Variant& s) {
	return os << "{t=" << s.t << ", d=" << s.d << ", m=" << s.m << '}';
}
// Vector.
std::ostream& operator<<(std::ostream& os, const std::vector<Variant>& s) {
	return display_vector(os, s);
}
// Unordered string map.
std::ostream& operator<<(std::ostream& os, const std::unordered_map<std::string,Variant>& s) {
	return display_unordered_map(os, s);
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
// Vector.
std::ostream& operator<<(std::ostream& os, const std::vector<InstToken>& s) {
	return display_vector(os, s);
}
// Unordered string map.
std::ostream& operator<<(std::ostream& os, const std::unordered_map<std::string,InstToken>& s) {
	return display_unordered_map(os, s);
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
// Vector.
std::ostream& operator<<(std::ostream& os, const std::vector<ExprToken>& s) {
	return display_vector(os, s);
}
// Unordered string map.
std::ostream& operator<<(std::ostream& os, const std::unordered_map<std::string,ExprToken>& s) {
	return display_unordered_map(os, s);
}




// Scope State.
// ------------

struct ScopeState {
	ScopeState* p = nullptr;
	std::unordered_map<std::string,Variant> d;
};


std::ostream& operator<<(std::ostream& os, const ScopeState& s) {
	os << "ScopeState{p=" << s.p << ", " << s.d << "}";
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
	return os << "Instruction{}";
}




// Operation.
// ----------

struct Operation {
	std::unordered_map<VariantType,std::vector<VariantType>> type_map;
	Variant (*exec)(Operation& op, ScopeState& state, Variant& first, Variant& second, std::string symbol) = nullptr;
};


// Unordered VariantDataType map.
std::ostream& operator<<(std::ostream& os, const std::unordered_map<VariantType,std::vector<VariantType>>& s) {
	return display_unordered_map(os, s);
}




// Misc utility functions.
// -----------------------


std::string trim_left(std::string& text, char ch) {
	int start = text.find_first_not_of(ch);
	if (start == std::string::npos) {return text;}
	int end = text.find_last_not_of(ch);
	if (end == std::string::npos) {return text;}
	return text.substr(start, (end-start+1));
}


unsigned int count_non_empty_strings(std::vector<std::string> items) {
	unsigned int count = 0;
	unsigned int items_len = items.size();
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
	if (int_str[i] == '1') {return true;}
	return false;
}


bool exists_in_strvec(std::vector<std::string>& v, std::string val) {
	return (std::find(v.begin(), v.end(), val) != v.end());
}


bool exists_in_vtvec(std::vector<VariantType>& v, VariantType val) {
	return (std::find(v.begin(), v.end(), val) != v.end());
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

	if (exists_in_vtvec(op.type_map.at(first.t), second.t) == false) {
		emit_error(err_operand_type_mismatch(op_str, get_variant_type_name(first.t), get_variant_type_name(second.t)));
		return;
	}
}
