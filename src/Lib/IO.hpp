#pragma once


// Called whenever the module is imported.
// This can be called multiple times.
Variant LIB_IO_init_exec(ScopeState& state, const std::vector<Variant>& args) {
	return VariantPresets.empty;
}


Variant LIB_IO_in_exec(ScopeState& state, const std::vector<Variant>& args) {
	std::string input_line;
	std::getline(std::cin, input_line);
	return Variant{STR, input_line};
}


Variant LIB_IO_out_exec(ScopeState& state, const std::vector<Variant>& args) {
	for (const Variant& var : args) {
		std::cout << var.d;
	}
	return VariantPresets.empty;
}


Variant LIB_IO_print_exec(ScopeState& state, const std::vector<Variant>& args) {
	for (const Variant& var : args) {
		std::cout << var.d << '\n';
	}
	return VariantPresets.empty;
}


// Define translations.
// --------------------


const Variant LIB_IO_init {
	MAP,
	(MAP_t){
		{"__t", VariantPresets.obj_type_f},
		{"__ret_t", VariantPresets.none_type_str},
		{"__ncall", Variant{FUNC, (NativeFunc_t)LIB_IO_init_exec}}
	},
	VariantMode_constant
};
const Variant LIB_IO_in {
	MAP,
	(MAP_t){
		{"__t", VariantPresets.obj_type_f},
		{"__ret_t", VariantPresets.str_type_str},
		{"__ncall", Variant{FUNC, (NativeFunc_t)LIB_IO_in_exec}}
	},
	VariantMode_constant
};
const Variant LIB_IO_out {
	MAP,
	(MAP_t){
		{"__t", VariantPresets.obj_type_f},
		{"__ret_t", VariantPresets.none_type_str},
		{"__ncall", Variant{FUNC, (NativeFunc_t)LIB_IO_out_exec}}
	},
	VariantMode_constant
};
const Variant LIB_IO_print {
	MAP,
	(MAP_t){
		{"__t", VariantPresets.obj_type_f},
		{"__ret_t", VariantPresets.none_type_str},
		{"__ncall", Variant{FUNC, (NativeFunc_t)LIB_IO_print_exec}}
	},
	VariantMode_constant
};


const Variant LIB_IO {
	MAP,
	(MAP_t){
		{"__name", Variant{STR, (STR_t)"IO", VariantMode_constant}},
		{"__init", LIB_IO_init},
		{"in", LIB_IO_in},
		{"out", LIB_IO_out},
		{"print", LIB_IO_print},
	}
};
