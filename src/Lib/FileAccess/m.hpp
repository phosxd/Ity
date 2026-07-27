#pragma once


// Called whenever the module is imported.
// This can be called multiple times.
Variant LIB_FileAccess_init(ScopeState& _state, const ARR_t& args) {
	return VariantPresets.empty;
}


Variant LIB_FileAccess_read_file_text(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.empty;
	const std::vector<VariantType> valid_types = {STR};
	if (not expect_arg_types(args[0], valid_types, 0)) return VariantPresets.none;
	const STR_t& path = AnyCast(STR_t,args[0].d);

	std::ifstream f (path, std::ios::in | std::ios::binary);
	if (not f.is_open()) return VariantPresets.none;
	const std::string& text = (std::ostringstream() << f.rdbuf()).str();
	f.close();

	return Variant{STR, (STR_t)text};
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_FileAccess {
	MAP, (MAP_t){
		{"__name",       Variant{STR, (STR_t)"FileAccess", VariantMode_constant}},
		{"__init",       NativeFuncTrans(NONE,   (NativeFunc_t)LIB_FileAccess_init)},
		{"read_file_text",   NativeFuncTrans(STR,    (NativeFunc_t)LIB_FileAccess_read_file_text)},
		//{"read_file_bytes",   NativeFuncTrans(STR,    (NativeFunc_t)LIB_FileAccess_read_file_bytes)},
	},
	VariantMode_constant
};
