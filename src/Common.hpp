#pragma once

// Variable declarations at the end of the file.




// VariantType.
// ------------

enum VariantType {
	// Meta types.
	PLACEHOLDER,
	INTERNAL,
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


#define AnyCast(T, var) std::any_cast<const T&>(var)
#define AnyCastV(T, var) std::any_cast<T>(var)


// Get string representation of a VariantType.
std::string get_variant_type_name(const VariantType& type) {
	switch (type) {
		case NONE: return "NONE";
		// Meta types.
		case PLACEHOLDER: return "NONE";
		case INTERNAL: return "NONE";
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
	if (name == "*")          return INFERRED;
	else if (name == "ANY")   return ANY;
	else if (name == "OP")    return OP;
	else if (name == "REF")   return REF;
	else if (name == "FUNC")  return FUNC;
	// Real types.
	else if (name == "BOOL")   return BOOL;
	else if (name == "INT")    return INT;
	else if (name == "FLOAT")  return FLOAT;
	else if (name == "STR")    return STR;
	else if (name == "ARR")    return ARR;
	else if (name == "MAP")    return MAP;
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

#pragma pack(1)
struct Variant {
	VariantType t = NONE;
	VariantData d = std::any();
	VariantMode m = VariantMode_dynamic_type;
};


using INT_t = int;
using FLOAT_t = double;
using STR_t = std::string;
using ARR_t = std::vector<Variant>;
using MAP_t = std::unordered_map<std::string,Variant>;


// Resolve VariantData to a real VariantType.
VariantType get_variant_data_type(const VariantData& d) {
	const std::type_info& t = d.type();
	if (t == typeid(bool))          return BOOL;
	else if (t == typeid(INT_t))    return INT;
	else if (t == typeid(FLOAT_t))  return FLOAT;
	else if (t == typeid(STR_t))    return STR;
	else if (t == typeid(ARR_t))    return ARR;
	else if (t == typeid(MAP_t))    return MAP;
	return NONE;
}


// Return true if `data` is applicable to `var`.
bool variant_data_type_matches(const VariantData& data, const Variant& var, const bool do_emit_error = true) {
	const VariantType& data_type = get_variant_data_type(data);
	if (var.m == VariantMode_dynamic_type || var.t == data_type) return true;
	if (do_emit_error) emit_error(ERR_assignment_type_mismatch, {get_variant_type_name(data_type), get_variant_type_name(var.t)});
	return false;
}


// Return the number of bytes that VariantData takes up.
size_t get_variant_data_size(const VariantData& s) {
	const std::type_info& t = s.type();
	if (t == typeid(INT_t))         return sizeof(AnyCast(INT_t,s));
	else if (t == typeid(FLOAT_t))  return sizeof(AnyCast(FLOAT_t,s));
	else if (t == typeid(STR_t))    return AnyCast(STR_t,s).size();

	else if (t == typeid(ARR_t)) {
		size_t sum;
		for (const Variant& var : AnyCast(ARR_t,s)) {
			sum += sizeof(var.t) + sizeof(var.m) + get_variant_data_size(var.d);
		}
		return sum;
	}
	else if (t == typeid(MAP_t)) {
		size_t sum;
		for (const auto& it : AnyCast(MAP_t,s)) {
			sum += it.first.size() + sizeof(it.second.t) + sizeof(it.second.m) + get_variant_data_size(it.second.d);
		}
		return sum;
	}

	return 0;
}


std::ostream& operator<<(std::ostream& os, const VariantData& s) {
	const std::type_info& t = s.type();
	if (not s.has_value()) os << "none";
	else if (t == typeid(bool)) {
		if (std::any_cast<bool>(s)) os << "true";
		else os << "false";
	}
	else if (t == typeid(INT_t)) os << AnyCast(INT_t,s);
	else if (t == typeid(FLOAT_t)) os << std::to_string(AnyCast(FLOAT_t,s)); // `std::cout` wont show the full precision by default, so we convert to string.
	else if (t == typeid(STR_t)) os << AnyCast(STR_t,s);
	else if (t == typeid(ARR_t)) os << AnyCast(ARR_t,s);
	else if (t == typeid(MAP_t)) os << AnyCast(MAP_t,s);
	return os;
}


// COMPARISON OPERATORS

inline void emit_operator_overload_error(const std::string& operation, const VariantData& a, const VariantData& b) {
	emit_error(ERR_operand_type_mismatch, {operation, get_variant_type_name(get_variant_data_type(a)), get_variant_type_name(get_variant_data_type(b))});
}


bool operator==(const VariantData& a, const VariantData& b) {
	const std::type_info& t1 = a.type();
	const std::type_info& t2 = b.type();
	// If a is string & b is string...
	if (t1 == typeid(STR_t) && t2 == typeid(STR_t)) return AnyCast(STR_t,a) == AnyCast(STR_t,b);
	// If a is array & b is array...
	else if (t1 == typeid(ARR_t) && t2 == typeid(ARR_t)) return AnyCast(ARR_t,a) == AnyCast(ARR_t,b);
	// If a is map & b is map...
	else if (t1 == typeid(MAP_t) && t2 == typeid(MAP_t)) return AnyCast(MAP_t,a) == AnyCast(MAP_t,b);

	// If a is bool & b is bool...
	else if (t1 == typeid(bool) && t2 == typeid(bool)) return AnyCast(bool,a) == AnyCast(bool,b);
	// If a is int...
	else if (t1 == typeid(INT_t)) {
		// If b is int...
		if (t2 == typeid(INT_t)) return AnyCast(INT_t,a) == AnyCast(INT_t,b);
		// If b is float...
		else if (t2 == typeid(FLOAT_t)) return AnyCast(INT_t,a) == AnyCast(FLOAT_t,b);
	}
	// If a is float...
	else if (t1 == typeid(FLOAT_t)) {;
		// If b is int...
		if (t2 == typeid(INT_t)) return AnyCast(FLOAT_t,a) == AnyCast(INT_t,b);
		// If b is float...
		else if (t2 == typeid(FLOAT_t)) return AnyCast(FLOAT_t,a) == AnyCast(FLOAT_t,b);
	}

	// Throw error is none matched.
	emit_operator_overload_error("Compare(==)", a,b);
	return false;
}


bool operator>(const VariantData& a, const VariantData& b) {
	const std::type_info& t1 = a.type();
	const std::type_info& t2 = b.type();
	// If a is int...
	if (t1 == typeid(INT_t)) {
		// If b is int...
		if (t2 == typeid(INT_t)) return AnyCast(INT_t,a) > AnyCast(INT_t,b);
		// If b is float...
		else if (t2 == typeid(FLOAT_t)) return AnyCast(INT_t,a) > AnyCast(FLOAT_t,b);
	}
	// If a is float...
	else if (t1 == typeid(FLOAT_t)) {;
		// If b is int...
		if (t2 == typeid(INT_t)) return AnyCast(FLOAT_t,a) > AnyCast(INT_t,b);
		// If b is float...
		else if (t2 == typeid(FLOAT_t)) return AnyCast(FLOAT_t,a) > AnyCast(FLOAT_t,b);
	}

	// Throw error is none matched.
	emit_operator_overload_error("Compare(>)", a,b);
	return false;
}


bool operator<(const VariantData& a, const VariantData& b) {
	const std::type_info& t1 = a.type();
	const std::type_info& t2 = b.type();
	// If a is int...
	if (t1 == typeid(INT_t)) {
		// If b is int...
		if (t2 == typeid(INT_t)) return AnyCast(INT_t,a) < AnyCast(INT_t,b);
		// If b is float...
		else if (t2 == typeid(FLOAT_t)) return AnyCast(INT_t,a) < AnyCast(FLOAT_t,b);
	}
	// If a is float...
	else if (t1 == typeid(FLOAT_t)) {;
		// If b is int...
		if (t2 == typeid(INT_t)) return AnyCast(FLOAT_t,a) < AnyCast(INT_t,b);
		// If b is float...
		else if (t2 == typeid(FLOAT_t)) return AnyCast(FLOAT_t,a) < AnyCast(FLOAT_t,b);
	}

	// Throw error is none matched.
	emit_operator_overload_error("Compare(<)", a,b);
	return false;
}


// MATH OPERATORS


ARR_t operator+(const ARR_t& a, const ARR_t& b) {
	ARR_t result = a; result.reserve(b.size());
	for (size_t i = 0; i < b.size(); i++) {
		result.push_back(b[i]);
	}
	return result;
}


VariantData operator+(const VariantData& a, const VariantData& b) {
	const std::type_info& t1 = a.type();
	const std::type_info& t2 = b.type();
	// If a is string & b is string...
	if (t1 == typeid(STR_t) && t2 == typeid(STR_t)) return AnyCast(STR_t,a) + AnyCast(STR_t,b);
	// If a is array & b is array...
	else if (t1 == typeid(ARR_t) && t2 == typeid(ARR_t)) return AnyCast(ARR_t,a) + AnyCast(ARR_t,b);
	// If a is int...
	else if (t1 == typeid(INT_t)) {
		// If b is int...
		if (t2 == typeid(INT_t)) return AnyCast(INT_t,a) + AnyCast(INT_t,b);
		// If b is float...
		else if (t2 == typeid(FLOAT_t)) return AnyCast(INT_t,a) + AnyCast(FLOAT_t,b);
	}
	// If a is float...
	else if (t1 == typeid(FLOAT_t)) {;
		// If b is int...
		if (t2 == typeid(INT_t)) return AnyCast(FLOAT_t,a) + AnyCast(INT_t,b);
		// If b is float...
		else if (t2 == typeid(FLOAT_t)) return AnyCast(FLOAT_t,a) + AnyCast(FLOAT_t,b);
	}

	// Throw error is none matched.
	emit_operator_overload_error("Arith(+)", a,b);
	return a;
}


VariantData operator-(const VariantData& a, const VariantData& b) {
	const std::type_info& t1 = a.type();
	const std::type_info& t2 = b.type();
	// If a is int...
	if (t1 == typeid(INT_t)) {
		// If b is int...
		if (t2 == typeid(INT_t)) return AnyCast(INT_t,a) - AnyCast(INT_t,b);
		// If b is float...
		else if (t2 == typeid(FLOAT_t)) return AnyCast(INT_t,a) - AnyCast(FLOAT_t,b);
	}
	// If a is float...
	else if (t1 == typeid(FLOAT_t)) {;
		// If b is int...
		if (t2 == typeid(INT_t)) return AnyCast(FLOAT_t,a) - AnyCast(INT_t,b);
		// If b is float...
		else if (t2 == typeid(FLOAT_t)) return AnyCast(FLOAT_t,a) - AnyCast(FLOAT_t,b);
	}

	// Throw error is none matched.
	emit_operator_overload_error("Arith(-)", a,b);
	return a;
}


VariantData operator*(const VariantData& a, const VariantData& b) {
	const std::type_info& t1 = a.type();
	const std::type_info& t2 = b.type();
	// If a is string & b is int.
	if (t1 == typeid(STR_t) && t2 == typeid(INT_t)) {
		const INT_t& b_val = AnyCast(INT_t,b);
		if (b_val < 0) {
			emit_error(ERR_cannot_multiply_by_negative, {"STR"});
			return a;
		}
		return AnyCast(STR_t,a) * b_val;
	}
	// If a is array & b is int.
	else if (t1 == typeid(ARR_t) && t2 == typeid(INT_t)) {
		const INT_t& b_val = AnyCast(INT_t,b);
		if (b_val < 0) {
			emit_error(ERR_cannot_multiply_by_negative, {"ARR"});
			return a;
		}
		ARR_t a_val = AnyCast(ARR_t,a);
		ARR_t sum; sum.reserve(a_val.size()*b_val);
		for (INT_t i = 0; i < b_val; i++) sum = sum+a_val;
		return sum;
	}
	// If a is int...
	else if (t1 == typeid(INT_t)) {
		// If b is int...
		if (t2 == typeid(INT_t)) return AnyCast(INT_t,a) * AnyCast(INT_t,b);
		// If b is float...
		else if (t2 == typeid(FLOAT_t)) return AnyCast(INT_t,a) * AnyCast(FLOAT_t,b);
	}
	// If a is float...
	else if (t1 == typeid(FLOAT_t)) {;
		// If b is int...
		if (t2 == typeid(INT_t)) return AnyCast(FLOAT_t,a) * AnyCast(INT_t,b);
		// If b is float...
		else if (t2 == typeid(FLOAT_t)) return AnyCast(FLOAT_t,a) * AnyCast(FLOAT_t,b);
	}

	// Throw error is none matched.
	emit_operator_overload_error("Arith(*)", a,b);
	return a;
}


VariantData operator/(const VariantData& a, const VariantData& b) {
	const std::type_info& t1 = a.type();
	const std::type_info& t2 = b.type();
	// If a is int...
	if (t1 == typeid(INT_t)) {
		// If b is int...
		if (t2 == typeid(INT_t)) return std::any_cast<const INT_t&>(a) / std::any_cast<const INT_t&>(b);
		// If b is float...
		else if (t2 == typeid(FLOAT_t)) return std::any_cast<const INT_t&>(a) / std::any_cast<const FLOAT_t&>(b);
	}
	// If a is float...
	else if (t1 == typeid(FLOAT_t)) {;
		// If b is int...
		if (t2 == typeid(INT_t)) return std::any_cast<const FLOAT_t&>(a) / std::any_cast<const INT_t&>(b);
		// If b is float...
		else if (t2 == typeid(FLOAT_t)) return std::any_cast<const FLOAT_t&>(a) / std::any_cast<const FLOAT_t&>(b);
	}

	// Throw error is none matched.
	emit_operator_overload_error("Arith(/)", a,b);
	return a;
}


VariantData operator%(const VariantData& a, const VariantData& b) {
	const std::type_info& t1 = a.type();
	const std::type_info& t2 = b.type();
	// If a is int & b is int...
	if (t1 == typeid(INT_t) && t2 == typeid(INT_t)) return AnyCast(INT_t,a) % AnyCast(INT_t,b);

	// Throw error is none matched.
	emit_operator_overload_error("Arith(%)", a,b);
	return a;
}


// VARIANT OVERLOADS.

std::ostream& operator<<(std::ostream& os, const Variant& s) {
	return os << s.d;
}



// ExprToken.
// ----------


enum ExprTokenType {
	ExprTokenType_variant,
	ExprTokenType_sequence,
};


#pragma pack(1)
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




// InstToken.
// ----------


#pragma pack(1)
struct InstToken {
	unsigned int i = 0;
	unsigned int ln = 0;
	unsigned int col = 0;
	std::vector<std::string> args;
	ExprToken expr;

	uint16_t composite_size = 0; // How large the composite instruction is. If `0`, is not a composite instruction.
	bool declarative_composite = false; // If true, the composite instruction contains variable declarations. This info is used to optimize scoping.

	std::string linked_inst = "";
	int32_t linked_inst_pos = 0;

	std::vector<VariantData> meta;
};


std::ostream& operator<<(std::ostream& os, const InstToken& s) {
	os << "{ln=" << s.ln << ", col=" << s.col;
	if (not s.args.empty()) os << ", args=" << s.args;
	if (not s.expr.seq.empty()) os << ", expr=" << s.expr;
	if (s.composite_size > 0) {os << ", composite_size=" << s.composite_size;}
	if (not s.linked_inst.empty()) {
		os << ", linked_inst=" << s.linked_inst;
		os << ", linked_inst_pos=" << s.linked_inst_pos;
	}
	os << '}';
	return os;
}



// Scope State.
// ------------

#pragma pack(1)
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

#pragma pack(1)
struct Instruction {
	uint8_t REQUIRED; // Required argument count,
	void (*exec)(ScopeState&, const Instruction*, InstToken&, const std::vector<std::string>&) = nullptr;
	bool is_composite;
	bool has_expr = false;
};


std::ostream& operator<<(std::ostream& os, const Instruction& s) {
	return os << "Instruction{}";
}




// Operation.
// ----------

struct Operation {
	void (*exec)(ScopeState&, Variant& first, Variant& second, const std::string& symbol, Variant& result, Variant*& result_ptr) = nullptr;
	Variant (*pre_exec)(ScopeState&, Variant& first, const std::string& symbol, bool& eval_second_operand) = nullptr;
};


std::ostream& operator<<(std::ostream& os, const Operation& s) {
	return os << "Operation{}";
}



// Variant conversion / creation functions.
// ----------------------------------------


std::string multiple_types_str(const std::vector<VariantType>& types) {
	std::string result;
	unsigned int i = 0;
	for (const VariantType& type : types) {
		if (i != 0) result += " or ";
		result += get_variant_type_name(type);
		i++;
	}
	return result;
}


VariantData get_literal_from_str(const VariantType& type, const std::string& str_val) {
	if (type == OP || type == REF || type == STR) return str_val;
	else if (type == BOOL) return str_val == "true";
	else if (type == INT) {
		if (is_int_str_32_in_range(str_val)) return (INT_t)std::stoi(str_val);
		emit_error(ERR_cannot_initialize_value, {str_val, "Number too large"});
		return std::any();
	}
	else if (type == FLOAT) return (FLOAT_t)std::stod(str_val);
	else return std::any();
}


bool var_to_bool(const Variant& var) {
	switch (var.t) {
		case BOOL:   return AnyCast(bool,var.d);
		case INT:    return (bool)AnyCast(INT_t,var.d);
		case FLOAT:  return (bool)AnyCast(FLOAT_t,var.d);
		case STR:    return AnyCast(STR_t,var.d) == "true";

		default: return false;
	}
}


FLOAT_t var_to_float(const Variant& var) {
	switch (var.t) {
		case BOOL:   return (FLOAT_t)AnyCast(bool,var.d);
		case INT:    return (FLOAT_t)AnyCast(INT_t,var.d);
		case FLOAT:  return AnyCast(FLOAT_t,var.d);
		case STR: {
			const STR_t& d = AnyCast(STR_t,var.d);
			if (d.size() == 0 || NUM.find(d[0]) == std::string::npos) return 0.0;
			return std::stod(d);
		}

		default: return 0.0;
	}
}


INT_t var_to_int(const Variant& var) {
	switch (var.t) {
		case BOOL:   return (INT_t)AnyCast(bool,var.d);
		case INT:    return AnyCast(INT_t,var.d);
		case FLOAT:  return (INT_t)AnyCast(FLOAT_t,var.d);
		case STR: {
			const STR_t& d = AnyCast(STR_t,var.d);
			if (d.size() == 0 || NUM.find(d[0]) == std::string::npos) return 0;
			return std::stoi(d);
		}

		default: return 0;
	}
}


STR_t var_to_str(const Variant& var) {
	switch (var.t) {
		case BOOL:   return std::to_string(AnyCast(bool,var.d));
		case INT:    return std::to_string(AnyCast(INT_t,var.d));
		case FLOAT:  return std::to_string(AnyCast(FLOAT_t,var.d));
		case STR:    return AnyCast(STR_t,var.d);

		default: return "";
	}
}



using NativeFunc_t = Variant(*)(ScopeState& state, const ARR_t& args);
struct VariantPresets_struct {
	const Variant empty       {NONE, std::any(), VariantMode_constant};
	const Variant obj_type_m  {STR, (STR_t)"m", VariantMode_constant};
	const Variant obj_type_f  {STR, (STR_t)"f", VariantMode_constant};

	const Variant any_type_str    {STR, (STR_t)"ANY", VariantMode_constant};
	const Variant none_type_str   {STR, (STR_t)"NONE", VariantMode_constant};
	const Variant bool_type_str   {STR, (STR_t)"BOOL", VariantMode_constant};
	const Variant int_type_str    {STR, (STR_t)"INT", VariantMode_constant};
	const Variant float_type_str  {STR, (STR_t)"FLOAT", VariantMode_constant};
	const Variant str_type_str    {STR, (STR_t)"STR", VariantMode_constant};
	const Variant arr_type_str    {STR, (STR_t)"ARR", VariantMode_constant};
	const Variant map_type_str    {STR, (STR_t)"MAP", VariantMode_constant};

	const Variant bool_true   {BOOL, true, VariantMode_constant};
	const Variant bool_false  {BOOL, false, VariantMode_constant};
};
const VariantPresets_struct VariantPresets;


// Translate a native function to a usable function object.
const Variant NativeFuncTrans(const VariantType& return_type, const NativeFunc_t& native_func) {
	return Variant{
		MAP,
		(MAP_t){
			{"__t", VariantPresets.obj_type_f},
			{"__rt", Variant{INTERNAL, return_type, VariantMode_constant}},
			{"__nc", Variant{FUNC, native_func}}
		},
		VariantMode_constant,
	};
}


// NativeFunc helper functions.
// ----------------------------

bool expect_arg_count(const ARR_t& args, const size_t& count) {
	if (args.size() == count) return true;
	emit_error(ERR_invalid_func_arg_count, {std::to_string(count), std::to_string(args.size())});
	return false;
}


bool expect_arg_types(const Variant& arg, const std::vector<VariantType>& types, const unsigned int arg_idx) {
	if (exists_in_vec(types, arg.t)) return true;
	emit_error(ERR_invalid_func_arg_type, {std::to_string(arg_idx), multiple_types_str(types), get_variant_type_name(arg.t)});
	return false;
}



// Constants.
// ----------

constexpr STR_t ItyVersionString = "0.0.2";
// Last number indicates release type:
//	0 = release.
//	1 = beta / pre-release.
//	2 = experimental / custom.
const std::vector<INT_t> ItyVersion = {0,0,2, 0};

constexpr STR_t OSName =
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
