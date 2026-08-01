#pragma once

#include <csignal>


// Called whenever the module is imported.
// This can be called multiple times.
Variant LIB_BI_init(ScopeState& state, const ARR_t& args) {
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
	if (not expect_arg_types(args[0], {INT}, 0) || not expect_arg_types(args[1], {MAP}, 1)) return VariantPresets.none;

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
	if (not expect_arg_types(args[0], {STR}, 0)) return VariantPresets.none;

	const char* command = AnyCast(STR_t,args[0].d).c_str();
	return Variant{INT, (INT_t)system(command)};
}


// Pause thread execution for the given number of seconds.
Variant LIB_BI_sleep(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.none;
	if (not expect_arg_types(args[0], {INT, FLOAT}, 0)) return VariantPresets.none;
	const Variant& var = args[0];

	FLOAT_t sleep_time;
	switch (var.t) {
		case INT: sleep_time = AnyCast(INT_t,var.d); break;
		case FLOAT: sleep_time = AnyCast(FLOAT_t,var.d); break;
		default: break;
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


// Return the type of the given Variant.
Variant LIB_BI_type(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.none;
	return Variant{INT, (INT_t)(args[0].t)};
}


// Return the length of the given array or string.
Variant LIB_BI_length(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.none;
	if (not expect_arg_types(args[0], {STR, ARR}, 0)) return VariantPresets.none;
	const Variant& var = args[0];

	switch (var.t) {
		case ARR: return Variant{INT, (int)(AnyCast(ARR_t,var.d).size()) }; break;
		case STR: return Variant(INT, (int)(AnyCast(STR_t,var.d).size()) ); break;
		default: return VariantPresets.none;
	}
}


// Return the number of bytes taken by the given variant.
Variant LIB_BI_size(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.none;
	return Variant{INT, (int)get_variant_size(args[0])};
}


// Return an array of integers from the given start, end, & step.
Variant LIB_BI_range(ScopeState& _state, const ARR_t& args) {
	if (args.size() == 0) {
		emit_error(ERR_invalid_func_arg_count, {"1+", "0"});
		return VariantPresets.none;
	}
	if (not expect_arg_types(args[0], {INT}, 0)) return VariantPresets.none;

	INT_t step = 1;
	INT_t start = 0;
	INT_t end = AnyCast(INT_t,args[0].d);
	if (args.size() > 1) {
		if (not expect_arg_types(args[1], {INT}, 1)) return VariantPresets.none;
		start = end;
		end = AnyCast(INT_t,args[1].d);
	}
	if (args.size() > 2) {
		if (not expect_arg_types(args[2], {INT}, 2)) return VariantPresets.none;
		step = AnyCast(INT_t,args[2].d);
	}


	ARR_t data; data.reserve(end);
	for (INT_t i = start; i < end; i += step) {
		data.push_back(Variant{INT, i});
	}

	return Variant{ARR, data};
}


// Return a random number in range of `min` & `max` integer arguments.
Variant LIB_BI_rand(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 2)) return VariantPresets.none;
	if (not expect_arg_types(args[0], {INT}, 0) || not expect_arg_types(args[1], {INT}, 1)) return VariantPresets.none;

	const INT_t& min = AnyCast(INT_t,args[0].d);
	const INT_t& max = AnyCast(INT_t,args[1].d);

	return Variant{INT, (std::rand() % (max-min+1) + min)};
}




// Type methods.
// -------------


// Pointer.
// --------

// Reassign pointer address.
Variant LIB_BI_tm_ptr_reassign(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 2)) return VariantPresets.none;
	if (not expect_arg_types(args[1], {PTR}, 1)) return VariantPresets.none;
	// Get data.
	Variant* data = AnyCastV(Variant*,args[0].d);
	// Throw error if data is constant.
	if (data->m == VariantMode_constant) {
		emit_error(ERR_cannot_change_constant);
		return VariantPresets.none;
	}
	// Reassign pointer.
	data->d = AnyCastV(Variant*,args[1].d);
	return VariantPresets.none;
}


// String.
// -------

// Returns the raw character code for the first character in the string.
Variant LIB_BI_tm_str_raw(ScopeState& _state, ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.none;
	// Get data.
	const STR_t& data = AnyCast(STR_t, AnyCastV(Variant*,args[0].d)->d );
	// Return integer representation.
	if (data.empty()) return Variant{INT, (INT_t)-1};
	return Variant{INT, (INT_t)data[0]};
}


// Array.
// ------

Variant LIB_BI_tm_arr_map_erase(ScopeState& _state, ARR_t& args) {
	if (not expect_arg_count(args, 2)) return VariantPresets.none;

	Variant* var = AnyCastV(Variant*,args[0].d);
	// Throw error if the variant is a constant.
	if (var->m == VariantMode_constant) {
		emit_error(ERR_cannot_change_constant);
		return VariantPresets.none;
	}

	if (var->t == ARR) {
		if (not expect_arg_types(args[1], {INT}, 1)) return VariantPresets.none;
		// Get data & index.
		ARR_t& data = AnyCastV(ARR_t, var->d );
		const INT_t& index = AnyCast(INT_t,args[1].d);
		// Erase item from array reference.
		data.erase(data.begin()+index);
	}

	else if (var->t == MAP) {
		if (not expect_arg_types(args[1], {STR}, 1)) return VariantPresets.none;
		// Get data & map.
		MAP_t& data = AnyCastV(MAP_t, var->d );
		const STR_t& key = AnyCast(STR_t,args[1].d);
		// Erase item from map reference.
		data.erase(key);
	}

	return VariantPresets.none;
}


// Map.
// ----


// Return array of keys in the `MAP`.
Variant LIB_BI_tm_map_keys(ScopeState& _state, ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.none;

	// Get data.
	const MAP_t& data = AnyCast(MAP_t, AnyCastV(Variant*,args[0].d)->d );
	// Get all keys.
	ARR_t keys; keys.reserve(data.size());
	for (const auto& it : data) {
		keys.push_back(Variant{STR, (STR_t)it.first});
	}
	// Return the keys.
	return Variant{ARR, keys};
}


// Return whether or not the `MAP` has the given key.
Variant LIB_BI_tm_map_has(ScopeState& _state, ARR_t& args) {
	if (not expect_arg_count(args, 2)) return VariantPresets.none;
	if (not expect_arg_types(args[1], {STR}, 1)) return VariantPresets.none;

	// Get data & key.
	const MAP_t& data = AnyCast(MAP_t, AnyCastV(Variant*,args[0].d)->d );
	const STR_t& key = AnyCast(STR_t,args[1].d);
	// Return whether or not the map has the key.
	if (data.find(key) != data.end()) return VariantPresets.bool_true;
	return VariantPresets.bool_false;
}


// Function.
// ---------

// Bind arguments to the function.
Variant LIB_BI_tm_func_bind(ScopeState& _state, ARR_t& args) {
	if (not expect_arg_count(args, 2)) return VariantPresets.none;
	if (not expect_arg_types(args[1], {ARR}, 1)) return VariantPresets.none;

	// Get data.
	MAP_t data = AnyCast(MAP_t, AnyCastV(Variant*,args[0].d)->d ); // Copy the function.
	// Add the given array to the function's bounded arguments.
	data["__ba"].d = AnyCast(ARR_t,data["__ba"].d) + AnyCast(ARR_t,args[1].d);
	// Return the new function.
	return Variant{MAP, data};
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_BI {
	MAP, (MAP_t){
		{"__name", Variant{STR, (STR_t)"BI", VariantMode_constant}},
		{"__init", NativeFuncTrans(NONE, (NativeFunc_t)LIB_BI_init)},

		// Type methods.
		{"__tm", Variant{
			MAP, (MAP_t){
				{"PTR:reassign", NativeFuncTrans(PTR, (NativeFunc_t)LIB_BI_tm_ptr_reassign)},

				{"STR:raw",    NativeFuncTrans(INT,   (NativeFunc_t)LIB_BI_tm_str_raw)},
				{"ARR:erase",  NativeFuncTrans(NONE,  (NativeFunc_t)LIB_BI_tm_arr_map_erase)},
				{"MAP:erase",  NativeFuncTrans(NONE,  (NativeFunc_t)LIB_BI_tm_arr_map_erase)},
				{"MAP:keys",   NativeFuncTrans(NONE,  (NativeFunc_t)LIB_BI_tm_map_keys)},
				{"MAP:has",    NativeFuncTrans(NONE,  (NativeFunc_t)LIB_BI_tm_map_has)},

				{"MAP(f):bind",  NativeFuncTrans(MAP,  (NativeFunc_t)LIB_BI_tm_func_bind)},
		}, VariantMode_locked_type }},


		// Type names.
		{"ANY",    VariantPresets.any_type_int},
		{"NONE",   VariantPresets.none_type_int},
		{"BOOL",   VariantPresets.bool_type_int},
		{"INT",    VariantPresets.int_type_int},
		{"FLOAT",  VariantPresets.float_type_int},
		{"STR",    VariantPresets.str_type_int},
		{"ARR",    VariantPresets.arr_type_int},
		{"MAP",    VariantPresets.map_type_int},

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
		{"type_name",  NativeFuncTrans(STR,   (NativeFunc_t)LIB_BI_type_name)},
		{"type",       NativeFuncTrans(INT,   (NativeFunc_t)LIB_BI_type)},
		{"length",     NativeFuncTrans(STR,   (NativeFunc_t)LIB_BI_length)},
		{"size",       NativeFuncTrans(INT,   (NativeFunc_t)LIB_BI_size)},
		{"range",      NativeFuncTrans(ARR,   (NativeFunc_t)LIB_BI_range)},
		{"rand",       NativeFuncTrans(ARR,   (NativeFunc_t)LIB_BI_rand)},

}, VariantMode_constant };
