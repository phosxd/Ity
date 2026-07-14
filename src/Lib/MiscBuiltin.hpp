#pragma once


// Called whenever the module is imported.
// This can be called multiple times.
Variant LIB_MISCBI_init(ScopeState& state, const std::vector<Variant>& args) {
	return VariantPresets.empty;
}


// Return the type of the given Variant, in string form.
Variant LIB_MISCBI_get_state(ScopeState& state, const std::vector<Variant>& args) {
	const unsigned int& args_len = args.size();
	if (args_len != 0) {
		emit_error(ERR_invalid_func_arg_count, {"0", std::to_string(args_len)});
		return VariantPresets.empty;
	}

	return Variant{MAP, state.d};
}


// Pause thread execution for the given number of seconds.
Variant LIB_MISCBI_sleep(ScopeState& state, const std::vector<Variant>& args) {
	const unsigned int& args_len = args.size();
	if (args_len != 1) {
		emit_error(ERR_invalid_func_arg_count, {"1", std::to_string(args_len)});
		return VariantPresets.empty;
	}

	float sleep_time;
	if (args[0].t == INT) sleep_time = std::any_cast<int>(args[0].d);
	else if (args[0].t == FLOAT) sleep_time = std::any_cast<float>(args[0].d);
	else {
		emit_error(ERR_invalid_func_arg_type, {"0", "INT or FLOAT", get_variant_type_name(args[0].t)});
		return VariantPresets.empty;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds( (int)(sleep_time*1000) ));
	return VariantPresets.empty;
}


// Return the type of the given Variant, in string form.
Variant LIB_MISCBI_type_name(ScopeState& state, const std::vector<Variant>& args) {
	const unsigned int& args_len = args.size();
	if (args_len != 1) {
		emit_error(ERR_invalid_func_arg_count, {"1", std::to_string(args_len)});
		return VariantPresets.empty;
	}

	return Variant{STR, get_variant_type_name(args[0].t)};
}


// Return the length of the given array or string.
Variant LIB_MISCBI_length(ScopeState& state, const std::vector<Variant>& args) {
	const unsigned int& args_len = args.size();
	if (args_len != 1) {
		emit_error(ERR_invalid_func_arg_count, {"1", std::to_string(args_len)});
		return VariantPresets.empty;
	}

	if (args[0].t == ARR) {
		const ARR_t& data = std::any_cast<ARR_t>(args[0].d);
		return Variant{INT, (int)data.size()};
	}
	else if (args[0].t == STR) {
		const STR_t& data = std::any_cast<STR_t>(args[0].d);
		return Variant(INT, (int)data.size());
	}

	emit_error(ERR_invalid_func_arg_type, {"0", "STR or ARR", get_variant_type_name(args[0].t)});
	return VariantPresets.empty;
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_MISCBI {
	MAP,
	(MAP_t){
		{"__name", Variant{STR, (STR_t)"MISCBI", VariantMode_constant}},
		{"__init", NativeFuncTrans(VariantPresets.none_type_str, (NativeFunc_t)LIB_MISCBI_init)},

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
				{"reset",    Variant{STR, ANSI::reset, VariantMode_constant}},
				{"bold",     Variant{STR, ANSI::bold, VariantMode_constant}},
				{"black",    Variant{STR, ANSI::black, VariantMode_constant}},
				{"red",      Variant{STR, ANSI::red, VariantMode_constant}},
				{"green",    Variant{STR, ANSI::green, VariantMode_constant}},
				{"orange",   Variant{STR, ANSI::orange, VariantMode_constant}},
				{"blue",     Variant{STR, ANSI::blue, VariantMode_constant}},
				{"purple",   Variant{STR, ANSI::purple, VariantMode_constant}},
				{"white",    Variant{STR, ANSI::white, VariantMode_constant}},
				{"yellow",   Variant{STR, ANSI::yellow, VariantMode_constant}},
			},VariantMode_constant
		}},

		// Utility functions.
		{"get_state",      NativeFuncTrans(VariantPresets.map_type_str,  (NativeFunc_t)LIB_MISCBI_get_state)},
		{"sleep",      NativeFuncTrans(VariantPresets.none_type_str,  (NativeFunc_t)LIB_MISCBI_sleep)},
		{"type_name",  NativeFuncTrans(VariantPresets.int_type_str,   (NativeFunc_t)LIB_MISCBI_type_name)},
		{"length",     NativeFuncTrans(VariantPresets.int_type_str,   (NativeFunc_t)LIB_MISCBI_length)}
	}
};
