#pragma once


// Called whenever the module is imported.
// This can be called multiple times.
Variant LIB_MISCBI_init(const ARR_t& args) {
	return VariantPresets.empty;
}


// Return the type of the given Variant, in string form.
Variant LIB_MISCBI_get_state(const ARR_t& args) {
	if (not expect_arg_count(args, 0)) return VariantPresets.empty;

	return Variant{MAP, ST.d};
}


// Pause thread execution for the given number of seconds.
Variant LIB_MISCBI_sleep(const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.empty;
	const Variant& var = std::move(args[0]);

	float sleep_time;
	if (var.t == INT) sleep_time = std::any_cast<int>(var.d);
	else if (var.t == FLOAT) sleep_time = std::any_cast<float>(var.d);
	else {
		emit_error(ERR_invalid_func_arg_type, {"0", "INT or FLOAT", get_variant_type_name(var.t)});
		return VariantPresets.empty;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds( (int)(sleep_time*1000) ));
	return VariantPresets.empty;
}


// Return the type of the given Variant, in string form.
Variant LIB_MISCBI_type_name(const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.empty;
	return Variant{STR, get_variant_type_name(args[0].t)};
}


// Return the length of the given array or string.
Variant LIB_MISCBI_length(const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VariantPresets.empty;
	const Variant& var = std::move(args[0]);

	if (var.t == ARR) {
		const ARR_t& data = std::any_cast<ARR_t>(var.d);
		return Variant{INT, (int)data.size()};
	}
	else if (var.t == STR) {
		const STR_t& data = std::any_cast<STR_t>(var.d);
		return Variant(INT, (int)data.size());
	}

	emit_error(ERR_invalid_func_arg_type, {"0", "STR or ARR", get_variant_type_name(var.t)});
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
	},
	VariantMode_constant
};
