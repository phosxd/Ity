#pragma once

#include <csignal>


// Called whenever the module is imported.
// This can be called multiple times.
Variant LIB_BI_init(ScopeState& _state, const ARR_t& args) {
	return VariantPresets.none;
}




std::unordered_map<int, std::vector<MAP_t>> LIB_BI_signal_functions;
ScopeState* LIB_BI_state = nullptr;

// Calls all script functions in `LIB_BI_signal_functions`.
// Gets executed when we receive a system signal.
void LIB_BI_on_signal_received(const int sig) {
	const std::vector<MAP_t>& signal_functions = LIB_BI_signal_functions[sig];
	// Iterate on each connected function for this signal...
	for (const MAP_t& func : signal_functions) {
		Variant args = Variant{ARR, (ARR_t){}};
		// Call the function.
		call_script_function(*LIB_BI_state, func, args);
	}
};


// Connect a system signal to a function.
Variant LIB_BI_signal(ScopeState& state, const ARR_t& args) {
	if (not expect_arg_count(args, 2)) return VariantPresets.none;
	if (args[0].t != INT) {
		emit_error(ERR_invalid_func_arg_type, {"0", "INT", get_variant_type_name(args[1].t)});
		return VariantPresets.none;
	}
	if (args[1].t != MAP) {
		emit_error(ERR_invalid_func_arg_type, {"1", "MAP(f)", get_variant_type_name(args[1].t)});
		return VariantPresets.none;
	}

	const int signal_number = (int)AnyCast(INT_t,args[0].d);
	const MAP_t& func = AnyCast(MAP_t,args[1].d);
	// Emit error if not a function object.
	if (AnyCast(STR_t,func.at("__t").d) != "f") {
		emit_error(ERR_invalid_func_arg_type, {"1", "MAP(f)", get_variant_type_name(args[1].t)});
		return VariantPresets.none;
	}

	LIB_BI_state = get_state_at_depth(state, 1); // Always run the function in the global state, even if it wasnt defined there.
	LIB_BI_signal_functions[signal_number].push_back(func); // Add function to array.

	// Connect signal...
	switch (signal_number) {
		case 2: signal(SIGINT, LIB_BI_on_signal_received); break;
		case 15: signal(SIGTERM, LIB_BI_on_signal_received); break;
	}
	return VariantPresets.none;
}




// Call a system comamnd. Returns the exit status code.
Variant LIB_BI_system(ScopeState& _state, const ARR_t& args) {
	if (safe_mode) {
		emit_error(ERR_disallowed_member_in_safe_mode, {"system"});
		return VariantPresets.none;
	}

	if (not expect_arg_count(args, 1)) return VariantPresets.none;
	const std::vector<VariantType> valid_types = {STR};
	if (not expect_arg_types(args[0], valid_types, 0)) return VariantPresets.none;

	const char* command = AnyCast(STR_t,args[0].d).c_str();
	return Variant{INT, (INT_t)system(command)};
}


// Pause thread execution for the given number of seconds.
Variant LIB_BI_sleep(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.none;
	const Variant& var = args[0];

	FLOAT_t sleep_time;
	switch (var.t) {
		case INT: sleep_time = AnyCast(INT_t,var.d); break;
		case FLOAT: sleep_time = AnyCast(FLOAT_t,var.d); break;
		default:
			emit_error(ERR_invalid_func_arg_type, {"0", "INT or FLOAT", get_variant_type_name(var.t)});
			return VariantPresets.none;
	}

	std::this_thread::sleep_for(std::chrono::microseconds( (int)(sleep_time*1000000) ));
	return VariantPresets.none;
}


// Return a copy of the current scope state.
Variant LIB_BI_get_state(ScopeState& state, const ARR_t& args) {
	if (not expect_arg_count(args, 0)) return VariantPresets.none;
	return Variant{MAP, state.d};
}


// Return the type of the given Variant, in string form.
Variant LIB_BI_type_name(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.none;
	return Variant{STR, get_variant_type_name(args[0].t)};
}


// Return the length of the given array or string.
Variant LIB_BI_length(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.none;
	const Variant& var = args[0];

	switch (var.t) {
		case ARR: return Variant{INT, (int)(AnyCast(ARR_t,var.d).size()) }; break;
		case STR: return Variant(INT, (int)(AnyCast(STR_t,var.d).size()) ); break;
		default:
			emit_error(ERR_invalid_func_arg_type, {"0", "STR or ARR", get_variant_type_name(var.t)});
			return VariantPresets.none;
	}
}


// Return the number of bytes taken by the given variant.
Variant LIB_BI_size(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.none;
	return Variant{INT, (int)get_variant_data_size(args[0].d)};
}




// Candidate functions.
// --------------------


Variant LIB_BI_candi_arr_erase(ScopeState& _state, ARR_t& args) {
	if (not expect_arg_count(args, 2)) return VariantPresets.none;
	if (args[1].t != INT) {
		emit_error(ERR_invalid_func_arg_type, {"1", "INT", get_variant_type_name(args[1].t)});
		return VariantPresets.none;
	}

	ARR_t& data = AnyCastV(ARR_t,args[0].d);
	const INT_t& index = AnyCast(INT_t,args[1].d);
	data.erase(data.begin()+index);

	return VariantPresets.none;
}


Variant LIB_BI_candi_map_erase(ScopeState& _state, ARR_t& args) {
	if (not expect_arg_count(args, 2)) return VariantPresets.none;
	if (args[1].t != STR) {
		emit_error(ERR_invalid_func_arg_type, {"1", "STR", get_variant_type_name(args[1].t)});
		return VariantPresets.none;
	}

	MAP_t& data = AnyCastV(MAP_t,args[0].d);
	const STR_t& key = AnyCast(STR_t,args[1].d);
	data.erase(key);

	return VariantPresets.none;
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_BI {
	MAP, (MAP_t){
		{"__name", Variant{STR, (STR_t)"BI", VariantMode_constant}},
		{"__init", NativeFuncTrans(NONE, (NativeFunc_t)LIB_BI_init)},

		// Candidates.
		{"__candi", Variant{INTERNAL, (Candidates_t){
			{ARR, {
				{"erase", NativeFuncTrans(NONE, (NativeFunc_t)LIB_BI_candi_arr_erase)},
			}},
			{MAP, {
				{"erase", NativeFuncTrans(NONE, (NativeFunc_t)LIB_BI_candi_map_erase)},
			}},
		}, VariantMode_constant }},


		// Type names.
		{"NONE",   VariantPresets.none_type_str},
		{"BOOL",   VariantPresets.bool_type_str},
		{"INT",    VariantPresets.int_type_str},
		{"FLOAT",  VariantPresets.float_type_str},
		{"STR",    VariantPresets.str_type_str},
		{"ARR",    VariantPresets.arr_type_str},
		{"MAP",    VariantPresets.map_type_str},

		// System signals.
		{"SIGNAL", Variant{
			MAP, (MAP_t){
				{"interrupt",   Variant{INT, (INT_t)2, VariantMode_constant}},   // Program interupt request.
				{"terminate",   Variant{INT, (INT_t)15, VariantMode_constant}},  // Program termination request.
		}, VariantMode_constant }},

		// ANSI codes.
		{"ANSI", Variant{
			MAP, (MAP_t){
				{"reset",    Variant{STR, ANSI::reset,   VariantMode_constant}},
				{"bold",     Variant{STR, ANSI::bold,    VariantMode_constant}},
				{"black",    Variant{STR, ANSI::black,   VariantMode_constant}},
				{"red",      Variant{STR, ANSI::red,     VariantMode_constant}},
				{"green",    Variant{STR, ANSI::green,   VariantMode_constant}},
				{"orange",   Variant{STR, ANSI::orange,  VariantMode_constant}},
				{"blue",     Variant{STR, ANSI::blue,    VariantMode_constant}},
				{"purple",   Variant{STR, ANSI::purple,  VariantMode_constant}},
				{"white",    Variant{STR, ANSI::white,   VariantMode_constant}},
				{"yellow",   Variant{STR, ANSI::yellow,  VariantMode_constant}},
				// Extra sequences.
				{"cursor_off",    Variant{STR, ANSI::cursor_off,    VariantMode_constant}},
				{"cursor_on",     Variant{STR, ANSI::cursor_on,     VariantMode_constant}},
				{"clear_screen",  Variant{STR, ANSI::clear_screen,  VariantMode_constant}},
		}, VariantMode_constant }},


		// Utility functions.
		{"signal",     NativeFuncTrans(NONE,  (NativeFunc_t)LIB_BI_signal)},
		{"system",     NativeFuncTrans(INT,   (NativeFunc_t)LIB_BI_system)},
		{"sleep",      NativeFuncTrans(NONE,  (NativeFunc_t)LIB_BI_sleep)},
		{"get_state",  NativeFuncTrans(MAP,   (NativeFunc_t)LIB_BI_get_state)},
		{"type_name",  NativeFuncTrans(INT,   (NativeFunc_t)LIB_BI_type_name)},
		{"length",     NativeFuncTrans(STR,   (NativeFunc_t)LIB_BI_length)},
		{"size",       NativeFuncTrans(INT,   (NativeFunc_t)LIB_BI_size)},

}, VariantMode_constant };
