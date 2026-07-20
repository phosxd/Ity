#pragma once


// Called whenever the module is imported.
// This can be called multiple times.
Variant LIB_FileAccess_init(ScopeState& _state, const ARR_t& args) {
	return VariantPresets.empty;
}


Variant LIB_FileAccess_open(ScopeState& _state, const ARR_t& args) {
	return VariantPresets.empty;
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_FileAccess {
	MAP, (MAP_t){
		{"__name",       Variant{STR, (STR_t)"FileAccess", VariantMode_constant}},
		{"__init",       NativeFuncTrans(NONE,   (NativeFunc_t)LIB_FileAccess_init)},
		{"open",         NativeFuncTrans(STR,    (NativeFunc_t)LIB_FileAccess_open)},
	},
	VariantMode_constant
};
