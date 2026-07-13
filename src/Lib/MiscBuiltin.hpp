#pragma once


// Called whenever the module is imported.
// This can be called multiple times.
Variant LIB_MISCBI_init_exec(ScopeState& state, const std::vector<Variant>& args) {
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


const Variant LIB_MISCBI {
	MAP,
	(MAP_t){
		{"__name", Variant{STR, (STR_t)"MISCBI", VariantMode_constant}},
		{"__init", LIB_MISCBI_init},
		{"NONE", VariantPresets.none_type_str},
		{"BOOL", VariantPresets.bool_type_str},
		{"INT", VariantPresets.int_type_str},
		{"FLOAT", VariantPresets.float_type_str},
		{"STR", VariantPresets.str_type_str}
	}
};
