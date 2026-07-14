#pragma once

// Variable declarations at the end of the file.




// Misc display overloads.
// -----------------------

// uint8_t
std::ostream& operator<<(std::ostream& os, const uint8_t& s) {
	return os << std::to_string(s); // Convert to string, otherwie displays as empty.
}


// vector
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& s) {
	os << '[';
	const unsigned int len = s.size();
	for (unsigned int i = 0; i < len; i++) {
		if (i != 0) {os << ", ";}
		os << s[i];
	}
	return os << ']';
}


// unordered_map
template<class T, class T2>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<T,T2>& s) {
	os << '{';
	unsigned int idx = 0;
	for (auto i:s) {
		if (idx != 0) {os << ", ";}
		os << "\"" << i.first << "\"" << ": " << i.second;
		idx++;
	}
	return os << '}';
}




// Misc equality overloads.
// ------------------------

// vector
template<class T, class T2>
bool is_vec_equal(const std::vector<T>& a, const std::vector<T2>& b) {
	const unsigned int a_len = a.size();
	const unsigned int b_len = b.size();
	if (a_len != b_len) {return false;}
	for (unsigned int i = 0; i < a_len; i++) {
		if (not (a[i] == b[i])) {return false;}
	}
	return true;
}




// VariantType.
// ------------

enum VariantType {
	// Meta types.
	INFERRED,
	ANY,
	OP,
	REF,
	FUNC,
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
std::string get_variant_type_name(const VariantType& type) {
	switch (type) {
		case NONE: return "NONE";
		// Meta types.
		case INFERRED: return "*";
		case ANY: return "ANY";
		case OP: return "OP";
		case REF: return "REF";
		case FUNC: return "FUNC";
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
VariantType get_variant_type_from_name(const std::string& name) {
	// Meta types.
	if (name == "*") { return INFERRED;}
	else if (name == "ANY") {return ANY;}
	else if (name == "OP") {return OP;}
	else if (name == "REF") {return REF;}
	else if (name == "FUNC") {return FUNC;}
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

using VariantData = std::any;



// Variant.
// --------

enum VariantMode {
	VariantMode_dynamic_type,
	VariantMode_constant,
	VariantMode_locked_type,
};

struct Variant {
	VariantType t = NONE;
	VariantData d = std::any();
	VariantMode m = VariantMode_dynamic_type;
};


using STR_t = std::string;
using ARR_t = std::vector<Variant>;
using MAP_t = std::unordered_map<std::string,Variant>;


// Resolve VariantData to a real VariantType.
VariantType get_variant_data_type(const VariantData& d) {
	const std::type_info& t = d.type();
	if (t == typeid(bool)) {return BOOL;}
	else if (t == typeid(int)) {return INT;}
	else if (t == typeid(float)) {return FLOAT;}
	else if (t == typeid(STR_t)) {return STR;}
	else if (t == typeid(ARR_t)) {return ARR;}
	else if (t == typeid(MAP_t)) {return MAP;}
	return NONE;
}


std::ostream& operator<<(std::ostream& os, const std::any& s) {
	const std::type_info& t = s.type();
	if (s.has_value() == false) {
		os << "none";
	}
	else if (t == typeid(bool)) {
		if (std::any_cast<bool>(s)) os << "true";
		else os << "false";
	}
	else if (t == typeid(int)) {
		os << std::any_cast<int>(s);
	}
	else if (t == typeid(float)) {
		os << std::to_string(std::any_cast<float>(s)); // `std::cout` wont show the full precision by default, so we convert to string.
	}
	else if (t == typeid(std::string)) {
		os << std::any_cast<std::string>(s);
	}
	else if (t == typeid(ARR_t)) {
		os << std::any_cast<ARR_t>(s);
	}
	else if (t == typeid(MAP_t)) {
		os << std::any_cast<MAP_t>(s);
	}
	return os;
}


// COMPARISON OPERATORS

bool operator==(const VariantData& a, const VariantData& b) {
	const std::type_info& t1 = a.type();
	const std::type_info& t2 = b.type();
	// If a is string & b is string...
	if (t1 == typeid(STR_t) && t2 == typeid(STR_t)) {
		return std::any_cast<STR_t>(a) == std::any_cast<STR_t>(b);
	}
	// If a is array & b is array...
	else if (t1 == typeid(ARR_t) && t2 == typeid(ARR_t)) {
		return std::any_cast<ARR_t>(a) == std::any_cast<ARR_t>(b);
	}
	// If a is map & b is map...
	else if (t1 == typeid(MAP_t) && t2 == typeid(MAP_t)) {
		return std::any_cast<MAP_t>(a) == std::any_cast<MAP_t>(b);
	}

	// If a is bool & b is bool...
	else if (t1 == typeid(bool) && t2 == typeid(bool)) {
		return std::any_cast<bool>(a) == std::any_cast<bool>(b);
	}
	// If a is int...
	else if (t1 == typeid(int)) {
		// If b is int...
		if (t2 == typeid(int)) {return std::any_cast<int>(a) == std::any_cast<int>(b);}
		// If b is float...
		else if (t2 == typeid(float)) {return std::any_cast<int>(a) == std::any_cast<float>(b);}
	}
	// If a is float...
	else if (t1 == typeid(float)) {;
		// If b is int...
		if (t2 == typeid(int)) {return std::any_cast<float>(a) == std::any_cast<int>(b);}
		// If b is float...
		else if (t2 == typeid(float)) {return std::any_cast<float>(a) == std::any_cast<float>(b);}
	}

	// Throw error is none matched.
	emit_error(ERR_operand_type_mismatch, {"Compare(==)", get_variant_type_name(get_variant_data_type(a)), get_variant_type_name(get_variant_data_type(b))});
	return false;
}


bool operator>(const VariantData& a, const VariantData& b) {
	const std::type_info& t1 = a.type();
	const std::type_info& t2 = b.type();
	// If a is int...
	if (t1 == typeid(int)) {
		// If b is int...
		if (t2 == typeid(int)) {return std::any_cast<int>(a) > std::any_cast<int>(b);}
		// If b is float...
		else if (t2 == typeid(float)) {return std::any_cast<int>(a) > std::any_cast<float>(b);}
	}
	// If a is float...
	else if (t1 == typeid(float)) {;
		// If b is int...
		if (t2 == typeid(int)) {return std::any_cast<float>(a) > std::any_cast<int>(b);}
		// If b is float...
		else if (t2 == typeid(float)) {return std::any_cast<float>(a) > std::any_cast<float>(b);}
	}

	// Throw error is none matched.
	emit_error(ERR_operand_type_mismatch, {"Compare(>)", get_variant_type_name(get_variant_data_type(a)), get_variant_type_name(get_variant_data_type(b))});
	return false;
}


bool operator<(const VariantData& a, const VariantData& b) {
	const std::type_info& t1 = a.type();
	const std::type_info& t2 = b.type();
	// If a is int...
	if (t1 == typeid(int)) {
		// If b is int...
		if (t2 == typeid(int)) {return std::any_cast<int>(a) < std::any_cast<int>(b);}
		// If b is float...
		else if (t2 == typeid(float)) {return std::any_cast<int>(a) < std::any_cast<float>(b);}
	}
	// If a is float...
	else if (t1 == typeid(float)) {;
		// If b is int...
		if (t2 == typeid(int)) {return std::any_cast<float>(a) < std::any_cast<int>(b);}
		// If b is float...
		else if (t2 == typeid(float)) {return std::any_cast<float>(a) < std::any_cast<float>(b);}
	}

	// Throw error is none matched.
	emit_error(ERR_operand_type_mismatch, {"Compare(<)", get_variant_type_name(get_variant_data_type(a)), get_variant_type_name(get_variant_data_type(b))});
	return false;
}


// MATH OPERATORS


ARR_t operator+(const ARR_t& a, const ARR_t& b) {
	ARR_t result = a;
	result.reserve(b.size());
	for (unsigned int i = 0; i < b.size(); i++) {
		result.push_back(b[i]);
	}
	return result;
}


VariantData operator+(const VariantData& a, const VariantData& b) {
	const std::type_info& t1 = a.type();
	const std::type_info& t2 = b.type();
	// If a is string & b is string...
	if (t1 == typeid(STR_t) && t2 == typeid(STR_t)) {
		return std::any_cast<STR_t>(a) + std::any_cast<STR_t>(b);
	}
	// If a is array & b is array...
	else if (t1 == typeid(ARR_t) && t2 == typeid(ARR_t)) {
		ARR_t a_val = std::any_cast<ARR_t>(a);
		ARR_t b_val = std::any_cast<ARR_t>(b);
		return a_val + b_val;
	}
	// If a is int...
	else if (t1 == typeid(int)) {
		// If b is int...
		if (t2 == typeid(int)) {return std::any_cast<int>(a) + std::any_cast<int>(b);}
		// If b is float...
		else if (t2 == typeid(float)) {return std::any_cast<int>(a) + std::any_cast<float>(b);}
	}
	// If a is float...
	else if (t1 == typeid(float)) {;
		// If b is int...
		if (t2 == typeid(int)) {return std::any_cast<float>(a) + std::any_cast<int>(b);}
		// If b is float...
		else if (t2 == typeid(float)) {return std::any_cast<float>(a) + std::any_cast<float>(b);}
	}

	// Throw error is none matched.
	emit_error(ERR_operand_type_mismatch, {"Arith(+)", get_variant_type_name(get_variant_data_type(a)), get_variant_type_name(get_variant_data_type(b))});
	return a;
}


VariantData operator-(const VariantData& a, const VariantData& b) {
	const std::type_info& t1 = a.type();
	const std::type_info& t2 = b.type();
	// If a is int...
	if (t1 == typeid(int)) {
		// If b is int...
		if (t2 == typeid(int)) {return std::any_cast<int>(a) - std::any_cast<int>(b);}
		// If b is float...
		else if (t2 == typeid(float)) {return std::any_cast<int>(a) - std::any_cast<float>(b);}
	}
	// If a is float...
	else if (t1 == typeid(float)) {;
		// If b is int...
		if (t2 == typeid(int)) {return std::any_cast<float>(a) - std::any_cast<int>(b);}
		// If b is float...
		else if (t2 == typeid(float)) {return std::any_cast<float>(a) - std::any_cast<float>(b);}
	}

	// Throw error is none matched.
	emit_error(ERR_operand_type_mismatch, {"Arith(-)", get_variant_type_name(get_variant_data_type(a)), get_variant_type_name(get_variant_data_type(b))});
	return a;
}


VariantData operator*(const VariantData& a, const VariantData& b) {
	const std::type_info& t1 = a.type();
	const std::type_info& t2 = b.type();
	// If a is string & b is int.
	if (t1 == typeid(STR_t) && t2 == typeid(int)) {
		int b_val = std::any_cast<int>(b);
		if (b_val < 0) {
			emit_error(ERR_cannot_multiply_by_negative, {"STR"});
			return a;
		}
		STR_t a_val = std::any_cast<STR_t>(a);
		STR_t sum; sum.reserve(a_val.size()*b_val);
		for (int i = 0; i < b_val; i++) {sum += a_val;}
		return sum;
	}
	// If a is array & b is int.
	else if (t1 == typeid(ARR_t) && t2 == typeid(int)) {
		int b_val = std::any_cast<int>(b);
		if (b_val < 0) {
			emit_error(ERR_cannot_multiply_by_negative, {"ARR"});
			return a;
		}
		ARR_t a_val = std::any_cast<ARR_t>(a);
		ARR_t sum; sum.reserve(a_val.size()*b_val);
		for (int i = 0; i < b_val; i++) {sum = sum+a_val;}
		return sum;
	}
	// If a is int...
	else if (t1 == typeid(int)) {
		// If b is int...
		if (t2 == typeid(int)) {return std::any_cast<int>(a) * std::any_cast<int>(b);}
		// If b is float...
		else if (t2 == typeid(float)) {return std::any_cast<int>(a) * std::any_cast<float>(b);}
	}
	// If a is float...
	else if (t1 == typeid(float)) {;
		// If b is int...
		if (t2 == typeid(int)) {return std::any_cast<float>(a) * std::any_cast<int>(b);}
		// If b is float...
		else if (t2 == typeid(float)) {return std::any_cast<float>(a) * std::any_cast<float>(b);}
	}

	// Throw error is none matched.
	emit_error(ERR_operand_type_mismatch, {"Arith(*)", get_variant_type_name(get_variant_data_type(a)), get_variant_type_name(get_variant_data_type(b))});
	return a;
}


VariantData operator/(const VariantData& a, const VariantData& b) {
	const std::type_info& t1 = a.type();
	const std::type_info& t2 = b.type();
	// If a is int...
	if (t1 == typeid(int)) {
		// If b is int...
		if (t2 == typeid(int)) {return std::any_cast<int>(a) / std::any_cast<int>(b);}
		// If b is float...
		else if (t2 == typeid(float)) {return std::any_cast<int>(a) / std::any_cast<float>(b);}
	}
	// If a is float...
	else if (t1 == typeid(float)) {;
		// If b is int...
		if (t2 == typeid(int)) {return std::any_cast<float>(a) / std::any_cast<int>(b);}
		// If b is float...
		else if (t2 == typeid(float)) {return std::any_cast<float>(a) / std::any_cast<float>(b);}
	}

	// Throw error is none matched.
	emit_error(ERR_operand_type_mismatch, {"Arith(/)", get_variant_type_name(get_variant_data_type(a)), get_variant_type_name(get_variant_data_type(b))});
	return a;
}


VariantData operator%(const VariantData& a, const VariantData& b) {
	const std::type_info& t1 = a.type();
	const std::type_info& t2 = b.type();
	// If a is int & b is int...
	if (t1 == typeid(int) && t2 == typeid(int)) {
		return std::any_cast<int>(a) % std::any_cast<int>(b);
	}

	// Throw error is none matched.
	emit_error(ERR_operand_type_mismatch, {"Arith(%)", get_variant_type_name(get_variant_data_type(a)), get_variant_type_name(get_variant_data_type(b))});
	return a;
}


// VARIANT OVERLOADS.

std::ostream& operator<<(std::ostream& os, const Variant& s) {
	return os << s.d;
}



// InstToken.
// ----------


struct InstToken {
	unsigned int i = 0;
	unsigned int ln = 0;
	unsigned int col = 0;
	std::vector<std::string> args;

	uint16_t composite_size = 0; // How large the composite instruction is. If `0`, is not a composite instruction.

	std::string linked_inst = "";
	int32_t linked_inst_pos = 0;

	std::vector<std::any> meta;
};


std::ostream& operator<<(std::ostream& os, const InstToken& s) {
	os << "{ln=" << s.ln << ", col=" << s.col << ", args=" << s.args;
	if (s.composite_size > 0) {os << ", composite_size=" << s.composite_size;}
	if (s.linked_inst.empty() == false) {
		os << ", linked_inst=" << s.linked_inst;
		os << ", linked_inst_pos=" << s.linked_inst_pos;
	}
	os << '}';
	return os;
}




// ExprToken.
// ----------


enum ExprTokenType {
	ExprTokenType_variant,
	ExprTokenType_sequence,
};


struct ExprToken {
	unsigned int ln = 0;
	unsigned int col = 0;
	ExprTokenType t = ExprTokenType_variant;
	Variant var;
	std::vector<ExprToken> seq;
};


std::ostream& operator<<(std::ostream& os, const ExprToken& s) {
	os << "{ln=" << s.ln << ", col=" << s.col;
	os << ", var=" << s.var;
	os << ", seq=" << s.seq;
	os << '}';
	return os;
}




// Scope State.
// ------------

struct ScopeState {
	ScopeState* p = nullptr;  // Parent scope state.
	MAP_t d;                  // Scope data.
};


std::ostream& operator<<(std::ostream& os, const ScopeState& s) {
	os << "ScopeState{p=" << s.p << ", d=" << s.d << "}";
	return os;
}




// Instruction.
// ------------

struct Instruction {
	uint8_t REQUIRED; // Required argument count,
	int8_t OPTIONAL; // Optional argument count.
	void (*exec)(const Instruction*, InstToken&, const std::vector<std::string>&) = nullptr;
	bool is_composite;
};


std::ostream& operator<<(std::ostream& os, const Instruction& s) {
	return os << "Instruction{}";
}




// Operation.
// ----------

struct Operation {
	Variant (*exec)(Variant& first, Variant& second, const std::string& symbol) = nullptr;
};


std::ostream& operator<<(std::ostream& os, const Operation& s) {
	return os << "Operation{}";
}




// Misc utility functions.
// -----------------------


bool str_ends_with(const std::string& text, const std::string& suffix) {
	return text.size() >= suffix.size() && (text.compare(text.size()-suffix.size(), suffix.size(), suffix) == 0);
}


// Returns the string with all instances of `ch` removed from the start of it.
std::string trim_left(const std::string& text, const char ch) {
	const size_t& text_len = text.size();
	if (text_len == 0) return text;
	if (text.at(0) != ch) return text;
	bool ended = false;
	std::string result; result.reserve(text_len);
	for (size_t i = 0; i < text_len; i++) {
		if (text.at(i) != ch) ended = true;
		if (ended) result.push_back(text.at(i));
	}
	return result;
}


// Joins all elements in the vector into a new string, with each element separated by the given `sep`.
std::string join_str(const std::vector<std::string>& vec, const std::string& sep) {
	const size_t& vec_len = vec.size();
	std::string result = vec.front();
	result.reserve(sep.size() * (vec_len-1));
	for (size_t i = 1; i < vec_len; i++) {
		result += sep + vec.at(i);
	}
	return result;
}


// Splits the `text` into a vector of strings, with each element separated by the given `sep`.
std::vector<std::string> split_str(const std::string& text, const char sep) {
	std::stringstream ss (text);
	std::vector<std::string> result;
	std::string item;
	while (std::getline(ss, item, sep)) {
		result.push_back(item);
	}
	return result;
}


// Returns the number of strings that are empty inside the given vector.
unsigned int count_non_empty_strings(const std::vector<std::string>& items) {
	const size_t& items_len = items.size();
	unsigned int count = 0;
	for (size_t i = 0; i < items_len; i++) {
		if (not std::move(items.at(i)).empty()) count++;
	}
	return count;
}


// Returns `false` if the string representation falls out of the 32-bit range for integers.
// NOTE: This is not an accurate check, it stops at 2,000,000,000 instead of the actual maximum.
bool is_int_str_32_in_range(std::string int_str) {
	int_str = trim_left(int_str, '0');
	if (int_str.size() == 0) return true;

	const bool negative = (int_str.at(0) == '-');
	size_t digits = int_str.size();
	if (negative) digits--;
	// If too many or not enough digits, return false.
	if (digits > 10) return false;
	else if (digits < 10) return true;

	uint8_t i = 0;
	if (negative) i++;
	if (int_str.at(i) == '1') return true;
	return false;
}


template<class T, class T2>
bool exists_in_vec(const std::vector<T>& v, const T2& val) {
	for (auto i:v) {
		if (i == val) {return true;}
	}
	return false;
}



using NativeFunc_t = Variant(*)(ScopeState& state, const ARR_t& args);
struct VariantPresets_struct {
	const Variant empty {NONE, std::any(), VariantMode_constant};
	const Variant obj_type_m {STR, (STR_t)"f", VariantMode_constant};
	const Variant obj_type_f {STR, (STR_t)"f", VariantMode_constant};

	const Variant none_type_str {STR, (STR_t)"NONE", VariantMode_constant};
	const Variant bool_type_str {STR, (STR_t)"BOOL", VariantMode_constant};
	const Variant int_type_str {STR, (STR_t)"INT", VariantMode_constant};
	const Variant float_type_str {STR, (STR_t)"FLOAT", VariantMode_constant};
	const Variant str_type_str {STR, (STR_t)"STR", VariantMode_constant};
	const Variant arr_type_str {STR, (STR_t)"ARR", VariantMode_constant};
	const Variant map_type_str {STR, (STR_t)"MAP", VariantMode_constant};
};
const VariantPresets_struct VariantPresets;

Variant NativeFuncTrans(const Variant& return_type, const NativeFunc_t& native_func) {
	return Variant{
		MAP,
		(MAP_t){
			{"__t", VariantPresets.obj_type_f},
			{"__ret_t", return_type},
			{"__ncall", Variant{FUNC, native_func}}
		},
		VariantMode_constant,
	};
}




// Ity global structure.

struct ItyStruct {
	std::vector<InstToken> (*tokenize)(const std::string& src) = nullptr;
	void (*exec)(std::vector<InstToken>& sequence, const size_t start_idx, const int end_idx) = nullptr;
};

ItyStruct Ity;



// Constants.
// ----------

constexpr std::string ItyVersionString = "0.0.1";
// Last number indicates release type:
//	0 = release.
//	1 = beta / pre-release.
//	2 = experimental.
const std::vector<int> ItyVersion = {0,0,1, 0};

constexpr std::string OSName =
#if _WIN32
	"windows"
#elif __linux__
	"linux"
#elif __APPLE__
	#include "TargetConditionals.h"
	#if TARGET_OS_MAC
		"apple_mac"
	#elif TARGET_OS_IPHONE
		"apple_iphone"
	#endif
#elif __unix__
	"unix"
#elif _POSIX_VERSION
	"posix"
#else
	"unknown"
#endif
;


constexpr unsigned int uint16_max = 65535;




// Variables.
// ----------

std::vector<InstToken> InstTokenSeq;
unsigned int execution_depth_max = 5000;
unsigned int execution_depth = 0;

int exec_jump_value = 0;
bool exec_jump_out = false;

// Modified by "Var" instruction & "Access" operation.
unsigned int func_arg_index = 0;

// Modified by "While", "If", & "End" instructions.
std::vector<InstToken*> scoped_tokens;
// Managed by `ScopeState*_ongoing_scopes` functions.
std::vector<std::vector<InstToken*>> scoped_tokens_stack;
