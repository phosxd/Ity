#pragma once


Variant LIB_TIME_init_exec(ScopeState& state, const std::vector<Variant>& args) {
	return Variant{};
}


// Define translations.
// --------------------


const Variant LIB_TIME_init {
	MAP,
	(MAP_t){
		{"__t", VariantPresets.obj_type_f},
		{"__ret_t", VariantPresets.none_type_str},
		{"__ncall", Variant{FUNC, (NativeFunc_t)LIB_TIME_init_exec}}
	},
	VariantMode_constant
};


const Variant LIB_TIME {
	MAP,
	(MAP_t){
		{"__name", Variant{STR, (STR_t)"TIME", VariantMode_constant}},
		{"__init__", LIB_TIME_init},
	}
};
