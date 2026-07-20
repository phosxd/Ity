#pragma once


// Called whenever the module is imported.
// This can be called multiple times.
Variant LIB_BI_init(ScopeState& _state, const ARR_t& args) {
	return VariantPresets.empty;
}


// Return a copy of the current scope state.
Variant LIB_BI_get_state(ScopeState& state, const ARR_t& args) {
	if (not expect_arg_count(args, 0)) return VariantPresets.empty;
	return Variant{MAP, state.d};
}


// Pause thread execution for the given number of seconds.
Variant LIB_BI_sleep(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.empty;
	const Variant& var = args[0];

	FLOAT_t sleep_time;
	switch (var.t) {
		case INT: sleep_time = AnyCast(INT_t,var.d); break;
		case FLOAT: sleep_time = AnyCast(FLOAT_t,var.d); break;
		default:
			emit_error(ERR_invalid_func_arg_type, {"0", "INT or FLOAT", get_variant_type_name(var.t)});
			return VariantPresets.empty;
	}

	std::this_thread::sleep_for(std::chrono::microseconds( (int)(sleep_time*1000000) ));
	return VariantPresets.empty;
}


// Return the type of the given Variant, in string form.
Variant LIB_BI_type_name(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.empty;
	return Variant{STR, get_variant_type_name(args[0].t)};
}


// Return the length of the given array or string.
Variant LIB_BI_length(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.empty;
	const Variant& var = args[0];

	switch (var.t) {
		case ARR: return Variant{INT, (int)(AnyCast(ARR_t,var.d).size()) }; break;
		case STR: return Variant(INT, (int)(AnyCast(STR_t,var.d).size()) ); break;
		default:
			emit_error(ERR_invalid_func_arg_type, {"0", "STR or ARR", get_variant_type_name(var.t)});
			return VariantPresets.empty;
	}
}


// Return the number of bytes taken by the given variant.
Variant LIB_BI_size(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.empty;
	return Variant{INT, (int)get_variant_data_size(args[0].d)};
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_BI {
	MAP, (MAP_t){
		{"__name", Variant{STR, (STR_t)"BI", VariantMode_constant}},
		{"__init", NativeFuncTrans(NONE, (NativeFunc_t)LIB_BI_init)},

		// Type names.
		{"NONE",   VariantPresets.none_type_str},
		{"BOOL",   VariantPresets.bool_type_str},
		{"INT",    VariantPresets.int_type_str},
		{"FLOAT",  VariantPresets.float_type_str},
		{"STR",    VariantPresets.str_type_str},
		{"ARR",    VariantPresets.arr_type_str},
		{"MAP",    VariantPresets.map_type_str},

		// ANSI codes.
		{"ANSI", Variant{
			MAP,
			(MAP_t){
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
			},VariantMode_constant
		}},

		// Utility functions.
		{"get_state",  NativeFuncTrans(MAP,   (NativeFunc_t)LIB_BI_get_state)},
		{"sleep",      NativeFuncTrans(NONE,  (NativeFunc_t)LIB_BI_sleep)},
		{"type_name",  NativeFuncTrans(INT,   (NativeFunc_t)LIB_BI_type_name)},
		{"length",     NativeFuncTrans(STR,   (NativeFunc_t)LIB_BI_length)},
		{"size",       NativeFuncTrans(INT,   (NativeFunc_t)LIB_BI_size)}
	},
	VariantMode_constant
};
