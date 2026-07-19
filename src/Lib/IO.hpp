#pragma once


// Called whenever the module is imported.
// This can be called multiple times.
Variant LIB_IO_init(ScopeState& _state, const ARR_t& args) {
	return VariantPresets.empty;
}


Variant LIB_IO_in(ScopeState& _state, const ARR_t& args) {
	std::string input_line;
	std::getline(std::cin, input_line);
	return Variant{STR, input_line};
}


Variant LIB_IO_out(ScopeState& _state, const ARR_t& args) {
	for (const Variant& var : args) std::cout << var.d;
	return VariantPresets.empty;
}


Variant LIB_IO_print(ScopeState& state, const ARR_t& args) {
	LIB_IO_out(state, args);
	std::cout << '\n';
	return VariantPresets.empty;
}


Variant LIB_IO_print_err(ScopeState& _state, const ARR_t& args) {
	for (const Variant& var : args) std::cerr << var.d;
	std::cerr << '\n';
	return VariantPresets.empty;
}


Variant LIB_IO_prompt(ScopeState& state, const ARR_t& args) {
	LIB_IO_out(state, args);
	return LIB_IO_in(state, {});
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_IO {
	MAP,
	(MAP_t){
		{"__name",       Variant{STR, (STR_t)"IO", VariantMode_constant}},
		{"__init",       NativeFuncTrans(NONE,   (NativeFunc_t)LIB_IO_init)},
		{"in",           NativeFuncTrans(STR,    (NativeFunc_t)LIB_IO_in)},
		{"out",          NativeFuncTrans(NONE,   (NativeFunc_t)LIB_IO_out)},
		{"print",        NativeFuncTrans(NONE,   (NativeFunc_t)LIB_IO_print)},
		{"print_err",    NativeFuncTrans(NONE,   (NativeFunc_t)LIB_IO_print_err)},
		{"prompt",       NativeFuncTrans(STR,    (NativeFunc_t)LIB_IO_prompt)},
	},
	VariantMode_constant
};
