#pragma once


// Called whenever the module is imported.
// This can be called multiple times.
Variant LIB_IO_init(ScopeState& _state, const ARR_t& args) {
	return VariantPresets.none;
}


// Wait for then return a response.
Variant LIB_IO_in(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 0)) return VariantPresets.none;
	std::string input_line;
	std::getline(std::cin, input_line);
	return Variant{STR, (STR_t)input_line};
}


// Wait for a key press then return it.
Variant LIB_IO_key_in(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 0)) return VariantPresets.none;

	// These are both hacky solutions but it gets the job done.
	#ifdef _WIN32
		HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
		DWORD mode; GetConsoleMode(handle, &mode);
		DWORD new_mode = mode; new_mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
		SetConsoleMode(handle, new_mode);
		char input;
		std::cin >> input;
		SetConsoleMode(handle, mode);
	#else
		system("stty raw");
		char input;
		std::cin >> input;
		system("stty -raw");
	#endif

	return Variant{STR, (STR_t)std::string(1,input)};
}


// Output text.
Variant LIB_IO_out(ScopeState& state, const ARR_t& args) {
	for (const Variant& var : args) std::cout << var.d;
	std::cout << std::flush; // Instantly print to the screen.
	return VariantPresets.none;
}


// Output text with a leading new line.
Variant LIB_IO_print(ScopeState& state, const ARR_t& args) {
	LIB_IO_out(state, args);
	std::cout << '\n';
	return VariantPresets.none;
}


// Output text to the error pipe.
Variant LIB_IO_print_err(ScopeState& _state, const ARR_t& args) {
	for (const Variant& var : args) std::cerr << var.d;
	std::cerr << '\n';
	return VariantPresets.none;
}


// Prompt the user with a message & return the response.
Variant LIB_IO_prompt(ScopeState& state, const ARR_t& args) {
	LIB_IO_out(state, args);
	return LIB_IO_in(state, {});
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_IO {
	MAP, (MAP_t){
		{"__name",       Variant{STR, (STR_t)"IO", VariantMode_constant}},
		{"__init",       NativeFuncTrans(NONE,   (NativeFunc_t)LIB_IO_init)},
		{"in",           NativeFuncTrans(STR,    (NativeFunc_t)LIB_IO_in)},
		{"key_in",       NativeFuncTrans(STR,    (NativeFunc_t)LIB_IO_key_in)},
		{"out",          NativeFuncTrans(NONE,   (NativeFunc_t)LIB_IO_out)},
		{"print",        NativeFuncTrans(NONE,   (NativeFunc_t)LIB_IO_print)},
		{"print_err",    NativeFuncTrans(NONE,   (NativeFunc_t)LIB_IO_print_err)},
		{"prompt",       NativeFuncTrans(STR,    (NativeFunc_t)LIB_IO_prompt)},
	},
	VariantMode_constant
};
