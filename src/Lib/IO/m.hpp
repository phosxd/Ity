#pragma once

#include <csignal>



static std::unordered_map<uint8_t, std::vector<FUNC_t>> LIB_IO_signal_functions;
static ItyState* LIB_IO_state = nullptr; // NOTE: Would prefer not to store this here, it's ugly & prone to breaking if/when async becomes a thing.

// Calls all script functions in `LIB_IO_signal_functions`.
// Gets executed when we receive a system signal.
static void LIB_IO_on_signal_received(const int sig) {
	// Iterate on each connected function for this signal & call it...
	for (const FUNC_t& func : LIB_IO_signal_functions[sig]) {
		Variant args = Variant{ARR, (ARR_t){}};
		call_function(*LIB_IO_state, func, args);
	}
};


// Connect a system signal to a function.
static Variant LIB_IO_signal(ItyState& state, const ARR_t& args) {
	if (not expect_arg_count(args, 2) || not expect_arg_types(args[0], {INT}, 0) || not expect_arg_types(args[1], {FUNC}, 1)) return Variant{};
	const uint8_t signal_number = (uint8_t)AnyCast(INT_t,args[0].d);

	LIB_IO_state = &state;
	if (LIB_IO_signal_functions[signal_number].size() == 0) signal(signal_number, LIB_IO_on_signal_received);
	LIB_IO_signal_functions[signal_number].push_back(AnyCast(FUNC_t,args[1].d));

	return Variant{};
}




// Wait for then return a response.
static Variant LIB_IO_in(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 0)) return Variant{};
	std::string input_line;
	std::getline(std::cin, input_line);
	return Variant{STR, (STR_t)input_line};
}


// Wait for a key press then return it.
static Variant LIB_IO_key_in(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 0)) return Variant{};

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
static Variant LIB_IO_out(ItyState& state, const ARR_t& args) {
	for (const Variant& var : args) std::cout << var;
	std::cout << std::flush; // Instantly print to the screen.
	return Variant{};
}


// Output text to the error pipe.
static Variant LIB_IO_err(ItyState& _state, const ARR_t& args) {
	for (const Variant& var : args) std::cerr << var;
	std::cerr << std::flush;
	return Variant{};
}


// Output text with a leading new line.
static Variant LIB_IO_print(ItyState& state, const ARR_t& args) {
	for (const Variant& var : args) std::cout << var;
	std::cout << '\n' << std::flush;
	return Variant{};
}


// Prompt the user with a message & return the response.
static Variant LIB_IO_prompt(ItyState& state, const ARR_t& args) {
	LIB_IO_out(state, args);
	return LIB_IO_in(state, {});
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_IO {
	MAP, (MAP_t){
		{"__name",  Variant{STR, (STR_t)"IO", VariantMode_constant}},
		{"__safe",  Variant{BOOL, true}},

		// System signals.
		{"SIGNAL", Variant{
			MAP, (MAP_t){
				{"abort",      Variant{INT, (INT_t)SIGABRT, VariantMode_constant}},
				{"illegal",    Variant{INT, (INT_t)SIGILL,  VariantMode_constant}},
				{"interrupt",  Variant{INT, (INT_t)SIGINT,  VariantMode_constant}},
				{"seg_fault",  Variant{INT, (INT_t)SIGSEGV, VariantMode_constant}},
				{"terminate",  Variant{INT, (INT_t)SIGTERM, VariantMode_constant}},
				{"quit",       Variant{INT, (INT_t)SIGQUIT, VariantMode_constant}},
				{"user1",      Variant{INT, (INT_t)SIGUSR1, VariantMode_constant}},
				{"user2",      Variant{INT, (INT_t)SIGUSR2, VariantMode_constant}},
			}, VariantMode_constant }},


		// Functions.
		{"signal",     NativeFuncTrans(NONE,  (NativeFunc_t)LIB_IO_signal)},

		{"in",         NativeFuncTrans(STR,   (NativeFunc_t)LIB_IO_in)},
		{"key_in",     NativeFuncTrans(STR,   (NativeFunc_t)LIB_IO_key_in)},
		{"out",        NativeFuncTrans(NONE,  (NativeFunc_t)LIB_IO_out)},
		{"err",        NativeFuncTrans(NONE,  (NativeFunc_t)LIB_IO_err)},
		{"print",      NativeFuncTrans(NONE,  (NativeFunc_t)LIB_IO_print)},
		{"prompt",     NativeFuncTrans(STR,   (NativeFunc_t)LIB_IO_prompt)},
	},
	VariantMode_constant
};
