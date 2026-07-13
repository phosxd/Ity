#pragma once


// Called whenever the module is imported.
// This can be called multiple times.
Variant LIB_MISCBI_init_exec(ScopeState& state, const std::vector<Variant>& args) {
	return VariantPresets.empty;
}


// Return the length of the given array or string.
Variant LIB_MISCBI_type_name_exec(ScopeState& state, const std::vector<Variant>& args) {
	const unsigned int& args_len = args.size();
	if (args_len != 1) {
		emit_error(ERR_invalid_func_arg_count, {"1", std::to_string(args_len)});
		return VariantPresets.empty;
	}

	return Variant{STR, get_variant_type_name(args[0].t)};
}


// Return the length of the given array or string.
Variant LIB_MISCBI_length_exec(ScopeState& state, const std::vector<Variant>& args) {
	const unsigned int& args_len = args.size();
	if (args_len != 1) {
		emit_error(ERR_invalid_func_arg_count, {"1", std::to_string(args_len)});
		return VariantPresets.empty;
	}

	if (args[0].t == ARR) {
		const ARR_t& data = std::any_cast<ARR_t>(args[0].d);
		return Variant{INT, (int)data.size()};
	}
	else if (args[0].t == STR) {
		const STR_t& data = std::any_cast<STR_t>(args[0].d);
		return Variant(INT, (int)data.size());
	}

	emit_error(ERR_invalid_func_arg_type, {"0", "STR or ARR", get_variant_type_name(args[0].t)});
	return VariantPresets.empty;
}


// Define translations.
// --------------------


const Variant LIB_MISCBI_init {
	MAP,
	(MAP_t){
		{"__t", VariantPresets.obj_type_f},
		{"__ret_t", VariantPresets.none_type_str},
		{"__ncall", Variant{FUNC, (NativeFunc_t)LIB_MISCBI_init_exec}}
	},
	VariantMode_constant
};
const Variant LIB_MISCBI_type_name {
	MAP,
	(MAP_t){
		{"__t", VariantPresets.obj_type_f},
		{"__ret_t", VariantPresets.int_type_str},
		{"__ncall", Variant{FUNC, (NativeFunc_t)LIB_MISCBI_type_name_exec}}
	},
	VariantMode_constant
};
const Variant LIB_MISCBI_length {
	MAP,
	(MAP_t){
		{"__t", VariantPresets.obj_type_f},
		{"__ret_t", VariantPresets.int_type_str},
		{"__ncall", Variant{FUNC, (NativeFunc_t)LIB_MISCBI_length_exec}}
	},
	VariantMode_constant
};


const Variant LIB_MISCBI {
	MAP,
	(MAP_t){
		{"__name", Variant{STR, (STR_t)"MISCBI", VariantMode_constant}},
		{"__init", LIB_MISCBI_init},

		// Type names.
		{"NONE", VariantPresets.none_type_str},
		{"BOOL", VariantPresets.bool_type_str},
		{"INT", VariantPresets.int_type_str},
		{"FLOAT", VariantPresets.float_type_str},
		{"STR", VariantPresets.str_type_str},
		{"ARR", VariantPresets.arr_type_str},
		{"MAP", VariantPresets.map_type_str},

		// Utility functions.
		{"type_name", LIB_MISCBI_type_name},
		{"length", LIB_MISCBI_length}
	}
};
