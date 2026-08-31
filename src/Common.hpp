#pragma once


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


#ifdef RUNTIME_DEBUG
std::ostream& operator<<(std::ostream& os, const ExprToken& s) {
	os << "{ln=" << s.ln << ", col=" << s.col;
	os << ", var=" << s.var;
	if (not s.seq.empty()) os << ", seq=" << s.seq;
	os << '}';
	return os;
}
#endif




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


#ifdef RUNTIME_DEBUG
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
#endif




// String hash stuff.
// ------------------


const std::hash<std::string> string_hasher;


struct HASHED_NAMES_struct {
	const size_t __AG   = string_hasher("__AG");
	const size_t __R    = string_hasher("__R");
	const size_t __tm__ = string_hasher("__tm__");

	const size_t __IMPORTED__ = string_hasher("__IMPORTED__");
};
const HASHED_NAMES_struct HASHED_NAMES;




// Instruction.
// ------------


#pragma pack(1)
struct Instruction {
	const uint8_t REQUIRED = 0;
	void (*exec)(ItyState&, InstToken&) = nullptr;
	const bool is_composite = false;
	const bool has_expr = false;
	const bool clear_args = false;
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
	switch (type) {
		case TREF: {
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
		case REF:
		case STR:  return str_val;
		case BOOL: return str_val == "true";
		case INT: {
			if (is_int_str_32_in_range(str_val)) return (INT_t)std::stoi(str_val);
			emit_error(ERR_cannot_initialize_value, {str_val, "Number too large"});
			return std::monostate();
		}
		case FLOAT: return (FLOAT_t)std::stod(str_val);
		default: return std::monostate();
	}
}


// Get the type of a `MAP` object.
const STR_t var_get_obj_type(const MAP_t& map) {
	if (const auto& it = map.find("__t"); it != map.end()) {
		const Variant& obj_type_var = it->second;
		if (obj_type_var.t != STR) {
			emit_error(ERR_unexpected, {"GetObjType", "Special member \"__t\" should be of type string."});
			return "";
		}
		return AnyCast(STR_t,obj_type_var.d);
	}
	return "m";
}


const Variant var_type_var(const VariantType type) {
	return Variant{INT, (INT_t)type, VariantMode_constant};
}


Variant none_var = {NONE, std::monostate(), VariantMode_constant};

// Collection of Variant presets used in various places within the codebase.
struct VariantPresets_struct {
	const Variant empty       {PLACEHOLDER, std::monostate(), VariantMode_constant};
	const Variant bool_true   {BOOL, true, VariantMode_constant};
	const Variant bool_false  {BOOL, false, VariantMode_constant};
};
const VariantPresets_struct VPS;


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


const bool ExpectArgs(const ARR_t& args, const std::vector<std::vector<VariantType>> types) {
	if (not expect_arg_count(args, types.size())) return false;
	size_t i = 0; for (const Variant& arg : args) {
		if (not expect_arg_types(arg, types[i], i)) return false;
		i++;
	}
	return true;
}




// TODO: move these into ItyState.

unsigned int execution_depth_max = 5000;
unsigned int execution_depth = 0;
