#pragma once


Variant LIB_IO_init_exec(ScopeState& state, const std::vector<Variant>& args) {
	return Variant{};
}


Variant LIB_IO_print_exec(ScopeState& state, const std::vector<Variant>& args) {
	for (const Variant& var : args) {
		std::cout << var.d << '\n';
	}
	return Variant{};
}


// Define translations.
// --------------------


const Variant LIB_IO_init {
	MAP,
	(MAP_t){
		{"__t", Variant{STR, (STR_t)"f"}},
		{"__ret_t", Variant{STR, (STR_t)"NONE"}},
		{"__ncall", Variant{FUNC, (NativeFunc_t)LIB_IO_init_exec}}
	},
	VariantMode_constant
};
const Variant LIB_IO_print {
	MAP,
	(MAP_t){
		{"__t", Variant{STR, (STR_t)"f"}},
		{"__ret_t", Variant{STR, (STR_t)"NONE"}},
		{"__ncall", Variant{FUNC, (NativeFunc_t)LIB_IO_print_exec}}
	},
	VariantMode_constant
};


const Variant LIB_IO {
	MAP,
	(MAP_t){
		{"__name", Variant{STR, (STR_t)"IO", VariantMode_constant}},
		{"__init__", LIB_IO_init},
		{"print", LIB_IO_print},
	}
};
