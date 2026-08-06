#pragma once

// Variable declarations at the end of the file.




#define AnyCast(T, var) std::get<T>(var)
#define AnyCastV(T, var) std::get<T>(var)




// VariantType.
// ------------


// Get string representation of a VariantType.
inline const std::string& get_variant_type_name(const VariantType& type) {return VARIANT_TYPE_NAMES.at(type);}


// Get VariantType from a string representation.
inline const VariantType get_variant_type_from_name(const std::string& name) {
	for (const auto& it : VARIANT_TYPE_NAMES) {
		if (it.second == name) return it.first;
	}
	return PLACEHOLDER;
}


std::ostream& operator<<(std::ostream& os, const VariantType& s) {return os << get_variant_type_name(s);}




// VariantData.
// ------------

// Forward declare these for use inside VariantData.
struct Variant;
struct ScopeState;
struct CompositeItem;

using INT_t = int32_t;
using UINT_t = uint32_t;
using FLOAT_t = double;
using STR_t = std::string;
using ARR_t = std::vector<Variant>;
using MAP_t = std::unordered_map<STR_t,Variant>;
using NativeFunc_t = Variant(*)(ScopeState& state, const ARR_t& args);

using VariantData = std::variant<
	Variant*,
	std::monostate,
	bool,
	INT_t,
	UINT_t,
	FLOAT_t,
	STR_t,
	ARR_t,
	MAP_t,

	// Internal types.
	uint16_t,
	VariantType,
	VariantMode,
	NativeFunc_t,
	OpSymbol,

	CompositeItem*,
	std::vector<CompositeItem>*
>;

using AnyMap_t = std::unordered_map<std::string, VariantData>;



// Variant.
// --------


#pragma pack(1)
struct Variant {
	VariantType t = NONE;
	VariantData d;
	VariantMode m = VariantMode_dynamic_type;
};


// Return true if `data` is applicable to `var`.
const bool variant_type_matches(const Variant& a, const Variant& b, const bool do_emit_error = true) {
	if (b.m == VariantMode_dynamic_type || b.t == a.t) return true;
	if (do_emit_error) emit_error(ERR_assignment_type_mismatch, {get_variant_type_name(a.t), get_variant_type_name(b.t)});
	return false;
}


// Return the number of bytes that Variant takes up.
const size_t get_variant_size(const Variant& var) {
	size_t size = sizeof(var.t) + sizeof(var.m);
	switch (var.t) {
		case INT:    {size += sizeof(AnyCast(INT_t,var.d)); break;}
		case FLOAT:  {size += sizeof(AnyCast(FLOAT_t,var.d)); break;}
		case STR:    {size += AnyCast(STR_t,var.d).size(); break;}

		case ARR: {
			const ARR_t& d = AnyCast(ARR_t,var.d);
			size_t sum = 0;
			for (const Variant& var : d) {
				sum += get_variant_size(var);
			}
			size += sizeof(d)+sum;
			break;
		}

		case MAP: {
			const MAP_t& d = AnyCast(MAP_t,var.d);
			size_t sum = 0;
			for (const auto& it : d) {
				sum += it.first.size() + get_variant_size(it.second);
			}
			size += sizeof(d)+sum;
			break;
		}
		default: break;
	}

	return size;
}


std::ostream& operator<<(std::ostream& os, const Variant& var) {
	if (var.d.index() == 0) return os << "none"; // Print "none" if unset.

	switch (var.t) {
		// Meta types.
		case OP:   {os << "OP:"   << (uint8_t)AnyCast(OpSymbol,var.d); break;}
		case TREF: {os << "TREF:" << AnyCast(STR_t,var.d); break;}

		// Real types.
		case NONE:  {os << "none";                                 break;}
		case REF:   {os << "REF:" << AnyCast(STR_t,var.d);         break;}
		case BOOL:  {os << (AnyCast(bool,var.d) ? "true":"false"); break;}
		case INT:   {os << AnyCast(INT_t,var.d);                   break;}
		case FLOAT: {os << std::to_string(AnyCast(FLOAT_t,var.d)); break;} // `std::cout` wont show the full precision by default, so we convert to string.
		case STR:   {os << AnyCast(STR_t,var.d);                   break;}

		case ARR: {
			os << '[';
			size_t i = 0;
			for (const Variant& it : AnyCast(ARR_t,var.d)) {
				if (i != 0) os << ", ";
				if (it.t == STR) os << '"' << it << '"';
				else os << it;
				i++;
			}
			os << ']';
			break;
		}

		case MAP: {
			os << '{';
			size_t idx = 0;
			for (auto& i : AnyCast(MAP_t,var.d)) {
				if (exists_in_vec(illegal_print_names, i.first)) continue;
				if (idx != 0) {os << ", ";}
				os << '"' << i.first << "\": ";
				if (i.second.t == STR) os << '"' << i.second << '"';
				else os << i.second;
				idx++;
			}
			os << '}';
			break;
		}
		default: break;
	}
	return os;
}


// COMPARISON OPERATORS

inline void emit_operator_overload_error(const std::string& operation, const Variant& a, const Variant& b) {
	emit_error(ERR_operand_type_mismatch, {operation, get_variant_type_name(a.t), get_variant_type_name(b.t)});
}


const bool operator==(const Variant& a, const Variant& b) {
	switch (a.t) {
		// If a is bool & b is bool...
		case BOOL: {
			if (b.t == BOOL) return AnyCast(bool,a.d) == AnyCast(bool,b.d);
			break;
		}
		// If a is int...
		case INT: {
			if (b.t == INT)        return AnyCast(INT_t,a.d) == AnyCast(INT_t,b.d);
			else if (b.t == FLOAT) return AnyCast(INT_t,a.d) == AnyCast(FLOAT_t,b.d);
			break;
		}
		// If a is float...
		case FLOAT: {
			if (b.t == INT)        return AnyCast(FLOAT_t,a.d) == AnyCast(INT_t,b.d);
			else if (b.t == FLOAT) return AnyCast(FLOAT_t,a.d) == AnyCast(FLOAT_t,b.d);
			break;
		}
		// If a is string & b is string...
		case STR: {
			if (b.t == STR) return AnyCast(STR_t,a.d) == AnyCast(STR_t,b.d);
			break;
		}
		// If a is array & b is array...
		case ARR: {
			if (b.t == ARR) return AnyCast(ARR_t,a.d) == AnyCast(ARR_t,b.d);
			break;
		}
		// If a is map & b is map...
		case MAP: {
			if (b.t == MAP) return AnyCast(MAP_t,a.d) == AnyCast(MAP_t,b.d);
			break;
		}
		default: break;
	}

	// Throw error is none matched.
	emit_operator_overload_error("Compare(==)", a,b);
	return false;
}


const bool operator>(const Variant& a, const Variant& b) {
	switch (a.t) {
		// If a is int...
		case INT: {
			if (b.t == INT)        return AnyCast(INT_t,a.d) > AnyCast(INT_t,b.d);
			else if (b.t == FLOAT) return AnyCast(INT_t,a.d) > AnyCast(FLOAT_t,b.d);
			break;
		}
		// If a is float...
		case FLOAT: {
			if (b.t == INT)        return AnyCast(FLOAT_t,a.d) > AnyCast(INT_t,b.d);
			else if (b.t == FLOAT) return AnyCast(FLOAT_t,a.d) > AnyCast(FLOAT_t,b.d);
			break;
		}
		default: break;
	}

	// Throw error is none matched.
	emit_operator_overload_error("Compare(>)", a,b);
	return false;
}


const bool operator<(const Variant& a, const Variant& b) {
	switch (a.t) {
		// If a is int...
		case INT: {
			if (b.t == INT)        return AnyCast(INT_t,a.d) < AnyCast(INT_t,b.d);
			else if (b.t == FLOAT) return AnyCast(INT_t,a.d) < AnyCast(FLOAT_t,b.d);
			break;
		}
		// If a is float...
		case FLOAT: {
			if (b.t == INT)        return AnyCast(FLOAT_t,a.d) < AnyCast(INT_t,b.d);
			else if (b.t == FLOAT) return AnyCast(FLOAT_t,a.d) < AnyCast(FLOAT_t,b.d);
		}
		default: break;
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


MAP_t operator+(const MAP_t& a, const MAP_t& b) {
	MAP_t result = a; result.reserve(b.size());
	for (const auto& it : b) {
		result[it.first] = it.second;
	}
	return result;
}


Variant operator+(const Variant& a, const Variant& b) {
	switch (a.t) {
		// If a is int...
		case INT: {
			if (b.t == INT)         return Variant{INT,   (AnyCast(INT_t,a.d) + AnyCast(INT_t,b.d))};
			else if (b.t == FLOAT)  return Variant{FLOAT, (AnyCast(INT_t,a.d) + AnyCast(FLOAT_t,b.d))};
		}
		// If a is float...
		case FLOAT: {
			if (b.t == INT)         return Variant{FLOAT, (AnyCast(FLOAT_t,a.d) + AnyCast(INT_t,b.d))};
			else if (b.t == FLOAT)  return Variant{FLOAT, (AnyCast(FLOAT_t,a.d) + AnyCast(FLOAT_t,b.d))};
			break;
		}
		// If a is string...
		case STR: {
			if (b.t == STR)       return Variant{STR, (AnyCast(STR_t,a.d) + AnyCast(STR_t,b.d))};
			else if (b.t == INT)  return Variant{STR, (AnyCast(STR_t,a.d) + char(AnyCast(INT_t,b.d)))}; // Add character to the string, from a code.
			break;
		}
		// If a is array & b is array...
		case ARR: {
			if (b.t == ARR) return Variant{ARR, (AnyCast(ARR_t,a.d) + AnyCast(ARR_t,b.d))};
			break;
		}
		// If a is map & b is map...
		case MAP: {
			if (b.t == MAP) return Variant{MAP, (AnyCast(MAP_t,a.d) + AnyCast(MAP_t,b.d))};
			break;
		}
		default: break;
	}

	// Throw error is none matched.
	emit_operator_overload_error("Arith(+)", a,b);
	return a;
}


Variant operator-(const Variant& a, const Variant& b) {
	switch (a.t) {
		// If a is int...
		case INT: {
			if (b.t == INT)         return Variant{INT,   (AnyCast(INT_t,a.d) - AnyCast(INT_t,b.d))};
			else if (b.t == FLOAT)  return Variant{FLOAT, (AnyCast(INT_t,a.d) - AnyCast(FLOAT_t,b.d))};
			break;
		}
		// If a is float...
		case FLOAT: {
			if (b.t == INT)         return Variant{FLOAT, (AnyCast(FLOAT_t,a.d) - AnyCast(INT_t,b.d))};
			else if (b.t == FLOAT)  return Variant{FLOAT, (AnyCast(FLOAT_t,a.d) - AnyCast(FLOAT_t,b.d))};
			break;
		}
		default: break;
	}

	// Throw error is none matched.
	emit_operator_overload_error("Arith(-)", a,b);
	return a;
}


Variant operator*(const Variant& a, const Variant& b) {
	switch (a.t) {
		// If a is int...
		case INT: {
			if (b.t == INT)         return Variant{INT,   (AnyCast(INT_t,a.d) * AnyCast(INT_t,b.d))};
			else if (b.t == FLOAT)  return Variant{FLOAT, (AnyCast(INT_t,a.d) * AnyCast(FLOAT_t,b.d))};
			break;
		}
		// If a is float...
		case FLOAT: {
			if (b.t == INT)         return Variant{FLOAT, (AnyCast(FLOAT_t,a.d) * AnyCast(INT_t,b.d))};
			else if (b.t == FLOAT)  return Variant{FLOAT, (AnyCast(FLOAT_t,a.d) * AnyCast(FLOAT_t,b.d))};
			break;
		}
		// If a is string & b is int.
		case STR: {
			if (b.t == INT) {
				const INT_t& b_val = AnyCast(INT_t,b.d);
				if (b_val < 0) {
					emit_error(ERR_cannot_multiply_by_negative, {"STR"});
					return a;
				}
				return Variant{STR, (AnyCast(STR_t,a.d) * b_val)};
			}
			break;
		}
		// If a is array & b is int.
		case ARR: {
			if (b.t == INT) {
				const INT_t& b_val = AnyCast(INT_t,b.d);
				if (b_val < 0) {
					emit_error(ERR_cannot_multiply_by_negative, {"ARR"});
					return a;
				}
				ARR_t a_val = AnyCastV(ARR_t,a.d);
				ARR_t sum; sum.reserve(a_val.size()*b_val);
				for (INT_t i = 0; i < b_val; i++) {
					for (auto& item : a_val) sum.push_back(item);
				};
				return Variant{ARR, std::move(sum)};
			}
			break;
		}
		default: break;
	}

	// Throw error is none matched.
	emit_operator_overload_error("Arith(*)", a,b);
	return a;
}


Variant operator/(const Variant& a, const Variant& b) {
	switch (a.t) {
		// If a is int...
		case INT: {
			// If b is int...
			if (b.t == INT) return Variant{INT, (AnyCast(INT_t,a.d) / AnyCast(INT_t,b.d))};
			// If b is float...
			else if (b.t == FLOAT) return Variant{FLOAT, (AnyCast(INT_t,a.d) / AnyCast(FLOAT_t,b.d))};
			break;
		}
		// If a is float...
		case FLOAT: {
			// If b is int...
			if (b.t == INT) return Variant{FLOAT, (AnyCast(FLOAT_t,a.d) / AnyCast(INT_t,b.d))};
			// If b is float...
			else if (b.t == FLOAT) return Variant{FLOAT, (AnyCast(FLOAT_t,a.d) / AnyCast(FLOAT_t,b.d))};
			break;
		}
		default: break;
	}

	// Throw error is none matched.
	emit_operator_overload_error("Arith(/)", a,b);
	return a;
}


Variant operator%(const Variant& a, const Variant& b) {
	// If a is int & b is int...
	if (a.t == INT && b.t == INT) return Variant{INT, (AnyCast(INT_t,a.d) % AnyCast(INT_t,b.d))};

	// Throw error is none matched.
	emit_operator_overload_error("Arith(%)", a,b);
	return a;
}



// ExprToken.
// ----------


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
	if (not s.seq.empty()) os << ", seq=" << s.seq;
	os << '}';
	return os;
}




// InstToken.
// ----------


struct Instruction;


#pragma pack(1)
struct InstToken {
	unsigned int i = 0;
	unsigned int ln = 0;
	unsigned int col = 0;
	InstSymbol symbol = InstSymbol__;
	std::vector<std::string> args;
	const Instruction* inst = nullptr;
	ExprToken expr;

	uint16_t composite_size = 0; // How large the composite instruction is. If `0`, is not a composite instruction.
	bool declarative_composite = false; // If true, the composite instruction contains variable declarations. This info is used to optimize scoping.

	InstSymbol linked_inst = InstSymbol__;
	int32_t linked_inst_pos = 0;

	std::vector<VariantData> meta;
};


std::ostream& operator<<(std::ostream& os, const InstToken& s) {
	os << "{ln=" << s.ln << ", col=" << s.col;
	if (not s.args.empty()) os << ", args=" << s.args;
	if (not s.expr.seq.empty()) os << ", expr=" << s.expr;
	if (s.composite_size > 0) {os << ", composite_size=" << s.composite_size;}
	if (s.linked_inst != InstSymbol__) {
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
	UINT_t id;
};




// Instruction.
// ------------


#pragma pack(1)
struct Instruction {
	const uint8_t REQUIRED = 0;
	void (*exec)(ScopeState&, InstToken&) = nullptr;
	const bool is_composite = false;
	const bool has_expr = false;
	void (*processor)(InstToken&, const AnyMap_t&, const unsigned int& ln, const unsigned int& col) = nullptr;
	void (*emergency_scope_exit)(InstToken*&) = nullptr;
};


#pragma pack(1)
struct CompositeItem {
	InstToken token;
	unsigned int index = 0;
	uint16_t size = 0;
	unsigned int ln = 0;
	unsigned int col = 0;
	bool declarative = false;
};





// Operation.
// ----------


struct Operation {
	void (*exec)(ScopeState&, Variant*& first, Variant*& second, const OpSymbol& symbol, Variant& result, Variant*& result_ptr) = nullptr;
	void (*pre_exec)(ScopeState&, Variant*& first, const OpSymbol& symbol, bool& eval_second_operand, Variant& result, Variant*& result_ptr) = nullptr;
};



// Variant conversion / creation functions.
// ----------------------------------------


const std::string multiple_types_str(const std::vector<VariantType>& types) {
	std::string result; result.reserve(types.size());
	unsigned int i = 0;
	for (const VariantType& type : types) {
		if (i != 0) result += " or ";
		result += get_variant_type_name(type);
		i++;
	}
	return result;
}


VariantData get_literal_from_str(const VariantType& type, const std::string& str_val) {
	if (type == OP || type == TREF || type == REF || type == STR) return str_val;
	else if (type == BOOL) return str_val == "true";
	else if (type == INT) {
		if (is_int_str_32_in_range(str_val)) return (INT_t)std::stoi(str_val);
		emit_error(ERR_cannot_initialize_value, {str_val, "Number too large"});
		return std::monostate();
	}
	else if (type == FLOAT) return (FLOAT_t)std::stod(str_val);
	else return std::monostate();
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
			if (d.size() == 0 || NUM.find(d[0]) == std::string::npos || not is_int_str_32_in_range(d)) return 0;
			return std::stoi(d);
		}

		default: return 0;
	}
}


STR_t var_to_str(const Variant& var) {
	switch (var.t) {
		case REF:    return "REF:"+AnyCast(STR_t,var.d);
		case BOOL:   return (AnyCast(bool,var.d)) ? "true" : "false";
		case INT:    return std::to_string(AnyCast(INT_t,var.d));
		case FLOAT:  return std::to_string(AnyCast(FLOAT_t,var.d));
		case STR:    return AnyCast(STR_t,var.d);

		default: return "";
	}
}


// Get the type of a `MAP` object.
const STR_t var_get_obj_type(const MAP_t& map) {
	STR_t obj_type = "m";
	if (map.find("__t") != map.end()) {
		const Variant& obj_type_var = map.at("__t");
		if (obj_type_var.t != STR) {
			emit_error(ERR_unexpected, {"var_get_obj_type", "Improper type of \"__t\" property."});
			return obj_type;
		}
		obj_type = AnyCast(STR_t,obj_type_var.d);
	}
	return obj_type;
}


Variant none_var = {NONE, std::monostate(), VariantMode_constant};

// Collection of Variant presets used in various places within the codebase.
struct VariantPresets_struct {
	const Variant empty       {PLACEHOLDER, std::monostate(), VariantMode_constant};
	const Variant none        = none_var;
	const Variant obj_type_m  {STR, (STR_t)"m", VariantMode_constant};
	const Variant obj_type_f  {STR, (STR_t)"f", VariantMode_constant};

	const Variant any_type_int    {INT, (INT_t)ANY, VariantMode_constant};
	const Variant ptr_type_int    {INT, (INT_t)PTR, VariantMode_constant};
	const Variant ref_type_int    {INT, (INT_t)REF, VariantMode_constant};
	const Variant none_type_int   {INT, (INT_t)NONE, VariantMode_constant};
	const Variant bool_type_int   {INT, (INT_t)BOOL, VariantMode_constant};
	const Variant int_type_int    {INT, (INT_t)INT, VariantMode_constant};
	const Variant float_type_int  {INT, (INT_t)FLOAT, VariantMode_constant};
	const Variant str_type_int    {INT, (INT_t)STR, VariantMode_constant};
	const Variant arr_type_int    {INT, (INT_t)ARR, VariantMode_constant};
	const Variant map_type_int    {INT, (INT_t)MAP, VariantMode_constant};

	const Variant bool_true   {BOOL, true, VariantMode_constant};
	const Variant bool_false  {BOOL, false, VariantMode_constant};
};
const VariantPresets_struct VariantPresets;


// Translate a native function to a usable function object.
const Variant NativeFuncTrans(const VariantType& return_type, const NativeFunc_t& native_func) {
	return Variant{
		MAP, (MAP_t){
			{"__t",   VariantPresets.obj_type_f},                            // Map type.
			{"__rt",  Variant{INTERNAL, return_type, VariantMode_constant}}, // Return type.
			{"__nc",  Variant{INTERNAL, native_func}},                       // Native callable.
			{"__ba",  Variant{ARR, (ARR_t){}}},                              // Bound args.
		},
		VariantMode_constant,
	};
}


// NativeFunc helper functions.
// ----------------------------

const bool expect_arg_count(const ARR_t& args, const size_t& count) {
	if (args.size() == count) return true;
	emit_error(ERR_invalid_func_arg_count, {std::to_string(count), std::to_string(args.size())});
	return false;
}


const bool expect_arg_types(const Variant& arg, std::vector<VariantType> types, const unsigned int arg_idx) {
	if (exists_in_vec(types, arg.t)) return true;
	emit_error(ERR_invalid_func_arg_type, {std::to_string(arg_idx), multiple_types_str(types), get_variant_type_name(arg.t)});
	return false;
}




// Constants.
// ----------

constexpr STR_t ItyVersionString = "0.1.1";
// Last number indicates release type:
//	0 = release.
//	1 = beta / pre-release.
//	2 = experimental / custom.
constexpr INT_t ItyVersion[4] = {0,1,1, 0};

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




// Variables.
// ----------

std::vector<InstToken> InstTokenSeq;
unsigned int execution_depth_max = 5000;
unsigned int execution_depth = 0;

int exec_jump_value = 0;
bool exec_jump_out = false;

// Modified by "While", "If", & "End" instructions.
std::vector<InstToken*> scoped_tokens;
// Managed by `ScopeState*_ongoing_scopes` functions.
std::vector<std::vector<InstToken*>> scoped_tokens_stack;
