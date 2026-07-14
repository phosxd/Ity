#pragma once


// Called whenever the module is imported.
// This can be called multiple times.
Variant LIB_IO_init(ScopeState& state, const std::vector<Variant>& args) {
	return VariantPresets.empty;
}


Variant LIB_IO_in(ScopeState& state, const std::vector<Variant>& args) {
	std::string input_line;
	std::getline(std::cin, input_line);
	return Variant{STR, input_line};
}


Variant LIB_IO_out(ScopeState& state, const std::vector<Variant>& args) {
	for (const Variant& var : args) {
		std::cout << var.d;
	}
	return VariantPresets.empty;
}


Variant LIB_IO_print(ScopeState& state, const std::vector<Variant>& args) {
	if (args.size() == 0) std::cout << '\n';
	for (const Variant& var : args) {
		std::cout << var.d << '\n';
	}
	return VariantPresets.empty;
}


Variant LIB_IO_prompt(ScopeState& state, const std::vector<Variant>& args) {
	if (args.size() == 0) std::cout << '\n';
	for (const Variant& var : args) {
		std::cout << var.d;
	}
	return LIB_IO_in(state, {});
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_IO {
	MAP,
	(MAP_t){
		{"__name", Variant{STR, (STR_t)"IO", VariantMode_constant}},
		{"__init", NativeFuncTrans(VariantPresets.none_type_str, (NativeFunc_t)LIB_IO_init)},
		{"in", NativeFuncTrans(VariantPresets.str_type_str, (NativeFunc_t)LIB_IO_in)},
		{"out", NativeFuncTrans(VariantPresets.none_type_str, (NativeFunc_t)LIB_IO_out)},
		{"print", NativeFuncTrans(VariantPresets.none_type_str, (NativeFunc_t)LIB_IO_print)},
		{"prompt", NativeFuncTrans(VariantPresets.str_type_str, (NativeFunc_t)LIB_IO_prompt)},
	}
};
