#pragma once


// ExprToken.
// ----------


#pragma pack(1)
struct ExprToken {
	uint32_t ln = 0;
	uint32_t col = 0;
	ExprTokenType t = ExprTokenType_variant;
	Variant var;
	std::vector<ExprToken> seq;
};


#pragma pack(1)
struct ExprState {
	STR_t path = ""; // Path to the Variant returned, if available.
	bool record_path = false; // If true, will record the path of the resulting Variant into `path`.
};


#ifdef RUNTIME_DEBUG
std::ostream& operator<<(std::ostream& os, const ExprToken& s) {
	os << "{ln=" << s.ln << ", col=" << s.col;
	os << ", v=" << get_variant_type_name(s.var.t);
	if (not s.seq.empty()) os << ", s=" << s.seq;
	os << '}';
	return os;
}
#endif




// InstToken.
// ----------


struct Instruction;


#pragma pack(1)
struct InstToken {
	uint32_t i = 0;
	uint32_t ln = 0;
	uint32_t col = 0;
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


#ifdef RUNTIME_DEBUG
std::ostream& operator<<(std::ostream& os, const InstToken& s) {
	os << "{ln=" << s.ln << ", col=" << s.col;
	if (not s.args.empty()) os << ", ag=" << s.args;
	if (not s.expr.seq.empty()) os << ", ex=" << s.expr;
	if (s.composite_size > 0) {os << ", cs=" << s.composite_size;}
	if (s.linked_inst != InstSymbol__) {
		os << ", li=" << s.linked_inst;
		os << ", lip=" << s.linked_inst_pos;
	}
	os << '}';
	return os;
}
#endif




// String hash stuff.
// ------------------


// const std::hash<std::string> string_hasher;
std::vector<std::string> string_hasher_id_map = {""}; // Index 0 is reserved empty.
var_key_t string_hasher(const std::string& str) {
	const var_key_t len = (var_key_t)string_hasher_id_map.size();
	var_key_t i = 1; while (i < len) {
		if (str == string_hasher_id_map[i]) return i;
		i++;
	}

	string_hasher_id_map.push_back(str);
	return i;
}


struct HASHED_NAMES_struct {
	const var_key_t __AG   = string_hasher("__AG");
	const var_key_t __R    = string_hasher("__R");
	const var_key_t __tm__ = string_hasher("__tm__");
	const var_key_t __IMPORTED__ = string_hasher("__IMPORTED__");
};
const HASHED_NAMES_struct HASHED_NAMES;




// Instruction.
// ------------


#pragma pack(1)
struct CompositeItem {
	InstToken token;
	uint32_t index = 0;
	uint16_t size = 0;
	uint32_t ln = 0;
	uint32_t col = 0;
	bool declarative = false;
};


#pragma pack(1)
struct InstProcArgs {
	const std::vector<CompositeItem>* composite_nest;
	const CompositeItem* last_comp_item;
	const uint32_t last_comp_item_dist;
	const uint32_t ln;
	const uint32_t col;
};


#pragma pack(1)
struct Instruction {
	const uint8_t REQUIRED = 0;
	void (*exec)(ItyState&, InstToken&) = nullptr;
	const bool is_composite = false;
	const bool has_expr = false;
	const bool clear_args = false;
	void (*processor)(InstToken&, const InstProcArgs&) = nullptr;
	void (*emergency_scope_exit)(InstToken*&) = nullptr;
};





// Operation.
// ----------


struct Operation {
	void (*exec)(ItyState&, ExprState&, Variant*& first, Variant*& second, const OpSymbol& symbol, Variant& result, Variant*& result_ptr) = nullptr;
	void (*pre_exec)(ItyState&, ExprState&, Variant*& first, const OpSymbol& symbol, bool& eval_second_operand, Variant& result, Variant*& result_ptr) = nullptr;
	const bool unary = false;
};



// Variant conversion / creation functions.
// ----------------------------------------


const std::string multiple_types_str(const std::vector<VariantType>& types) {
	std::string result; result.reserve(types.size());
	uint32_t i = 0;
	for (const VariantType& type : types) {
		if (i != 0) result += " or ";
		result += get_variant_type_name(type);
		i++;
	}
	return result;
}


VariantData get_literal_from_str(const VariantType& type, const std::string& str_val) {
	switch (type) {
		case VT_TREF: {
			return TREF_t{
				.str = str_val,
				.key = string_hasher(str_val),
			};
		}
		case VT_REF:
		case VT_STR:  return str_val;
		case VT_BOOL: return str_val == "true";
		// TODO: replace number parsing functions with custom solution that DOESNT CRASH WHEN IT GOES OUT OF RANGE. Ahem.
		case VT_INT64:   return (int64_t)std::stol(str_val);
		case VT_UINT64:  return (uint64_t)std::stod(str_val);
		case VT_FLOAT64: return (FLOAT64_t)std::stod(str_val);
		default: return std::monostate();
	}
}


// Get the type of a `MAP` object.
const STR_t var_get_obj_type(const MAP_t& map) {
	if (const auto& it = map.find("__t"); it != map.end()) {
		const Variant& obj_type_var = it->second;
		if (obj_type_var.t != VT_STR) {
			emit_error(ERR_unexpected, {"GetObjType", "Special member \"__t\" should be of type string."});
			return "";
		}
		return AnyCast(STR_t,obj_type_var.d);
	}
	return "m";
}


const Variant var_type_var(const VariantType& type) {
	return Variant{VT_UINT8, (uint8_t)type, VM_const};
}


const Variant new_none() {
	return Variant{VT_NONE, std::monostate(), VM_dynamic_type};
}


Variant none_var = {VT_NONE, std::monostate(), VM_const};

// Collection of Variant presets used in various places within the codebase.
struct VariantPresets_struct {
	const Variant none        {VT_NONE, std::monostate(), VM_const};
	const Variant empty       {VT_PLACEHOLDER, std::monostate(), VM_const};
	const Variant bool_true   {VT_BOOL, true, VM_const};
	const Variant bool_false  {VT_BOOL, false, VM_const};
};
const VariantPresets_struct VPS;


// Translate a native function to a usable function object.
const Variant NativeFuncTrans(const VariantType& return_type, const NativeFunc_t& native_func, const std::vector<std::vector<VariantType>> args_schema={}) {
	return Variant{
		VT_FUNC, (FUNC_t){
			.return_type = return_type,
			.bound_args = (ARR_t){},
			.native_callable = native_func,
			.args_schema = args_schema
		},
		VM_const,
	};
}


// NativeFunc helper functions.
// ----------------------------

const bool expect_arg_count(const ARR_t& args, const size_t& count) {
	if (args.size() == count) return true;
	emit_error(ERR_invalid_func_arg_count, {std::to_string(count), std::to_string(args.size())});
	return false;
}


const bool expect_arg_types(const Variant& arg, std::vector<VariantType> types, const size_t& arg_idx) {
	if (exists_in_vec(types, arg.t)) return true;
	emit_error(ERR_invalid_func_arg_type, {std::to_string(arg_idx), multiple_types_str(types), get_variant_type_name(arg.t)});
	return false;
}
