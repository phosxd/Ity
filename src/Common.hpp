#pragma once

// Variable declarations at the end of the file.



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
	if (s.composite_size > 0) {os << ", comp_size=" << s.composite_size;}
	if (s.linked_inst != InstSymbol__) {
		os << ", linked_inst=" << s.linked_inst;
		os << ", linked_inst_pos=" << s.linked_inst_pos;
	}
	os << '}';
	return os;
}




// String hash stuff.
// ------------------


const std::hash<std::string> string_hasher;


struct HASHED_NAMES_struct {
	const size_t __AG;
	const size_t __R;
	const size_t __tm__;
};
const HASHED_NAMES_struct HASHED_NAMES {
	string_hasher("__AG"),
	string_hasher("__R"),
	string_hasher("__tm__"),
};




// Instruction.
// ------------


#pragma pack(1)
struct Instruction {
	const uint8_t REQUIRED = 0;
	void (*exec)(ItyState&, InstToken&) = nullptr;
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
	void (*exec)(ItyState&, Variant*& first, Variant*& second, const OpSymbol& symbol, Variant& result, Variant*& result_ptr) = nullptr;
	void (*pre_exec)(ItyState&, Variant*& first, const OpSymbol& symbol, bool& eval_second_operand, Variant& result, Variant*& result_ptr) = nullptr;
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
	if (type == TREF) {
		const STR_t& real_name = trim_left(trim_left(str_val,'@'),'~');
		uint8_t mode = 0;
		if (str_val[0] == '@') mode = 1;
		if (str_val[0] == '~') mode = 2;
		return TREF_t{
			.str  = real_name,
			.hash = string_hasher(real_name),
			.mode = mode,
		};
	}

	else if (type == REF || type == STR) return str_val;
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
	if (const auto& it = map.find("__t"); it != map.end()) {
		const Variant& obj_type_var = it->second;
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
		FUNC, (FUNC_t){
			.return_type = return_type,
			.bound_args = (ARR_t){},
			.native_callable = native_func
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

constexpr STR_t ItyVersionString = "0.2.0";
// Last number indicates release type:
//	0 = release.
//	1 = beta / pre-release.
//	2 = experimental / custom.
constexpr INT_t ItyVersion[4] = {0,2,0, 1};

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


const std::string GLOBAL_SHARE_PATH =
#if _WIN32
	"%LOCALAPPDATA%/ity/"
#elif __APPLE__
	#include "TargetConditionals.h"
	#if TARGET_OS_MAC
		"~/Library/Application Support/ity/"
	#elif TARGET_OS_IPHONE
		"Documents/ity/"
	#endif
#else
	"/usr/local/share/ity/"
#endif
;




// Variables.
// ----------

unsigned int execution_depth_max = 5000;
unsigned int execution_depth = 0;

int exec_jump_value = 0;
bool exec_jump_out = false;
