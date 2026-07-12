#pragma once


Variant LIB_TIME_init_exec(ScopeState& state, const std::vector<Variant>& args) {
	return VariantPresets.empty;
}


Variant LIB_TIME_get_time(ScopeState& state, const std::vector<Variant>& args, const unsigned int mode) {
	const unsigned int& args_len = args.size();
	if (args_len != 1) {
		emit_error(ERR_invalid_func_arg_count, {"1", std::to_string(args_len)});
		return VariantPresets.empty;
	}

	if (args[0].t != STR) {
		emit_error(ERR_invalid_func_arg_type, {"0", "STR", get_variant_type_name(args[0].t)});
		return VariantPresets.empty;
	}
	const STR_t& precision = std::any_cast<STR_t>(args[0].d);

	// Initialize clocks.
	auto clock_1 = Clock::now();
	std::chrono::time_point<std::chrono::high_resolution_clock> clock_2;
	if (mode == 1) {clock_2 = clock_start;}

	// Get value.
	int result = 0;
	if (precision == "us") {
		result = std::chrono::duration_cast<std::chrono::microseconds>(clock_1-clock_2).count();
	}
	else if (precision == "ms") {
		result = std::chrono::duration_cast<std::chrono::milliseconds>(clock_1-clock_2).count();
	}
	else if (precision == "s") {
		result = std::chrono::duration_cast<std::chrono::seconds>(clock_1-clock_2).count();
	}
	else if (precision == "m") {
		result = std::chrono::duration_cast<std::chrono::minutes>(clock_1-clock_2).count();
	}
	else if (precision == "h") {
		result = std::chrono::duration_cast<std::chrono::hours>(clock_1-clock_2).count();
	}
	else if (precision == "d") {
		result = std::chrono::duration_cast<std::chrono::days>(clock_1-clock_2).count();
	}
	else if (precision == "w") {
		result = std::chrono::duration_cast<std::chrono::weeks>(clock_1-clock_2).count();
	}
	else if (precision == "M") {
		result = std::chrono::duration_cast<std::chrono::months>(clock_1-clock_2).count();
	}
	else if (precision == "y") {
		result = std::chrono::duration_cast<std::chrono::years>(clock_1-clock_2).count();
	}

	return Variant{INT, result};
}


Variant LIB_TIME_system_now_exec(ScopeState& state, const std::vector<Variant>& args) {
	return LIB_TIME_get_time(state, args, 0);
}


Variant LIB_TIME_now_exec(ScopeState& state, const std::vector<Variant>& args) {
	return LIB_TIME_get_time(state, args, 1);
}


// Define translations.
// --------------------


const Variant LIB_TIME_init {
	MAP,
	(MAP_t){
		{"__t", VariantPresets.obj_type_f},
		{"__ret_t", VariantPresets.none_type_str},
		{"__ncall", Variant{FUNC, (NativeFunc_t)LIB_TIME_init_exec}}
	},
	VariantMode_constant
};
const Variant LIB_TIME_system_now {
	MAP,
	(MAP_t){
		{"__t", VariantPresets.obj_type_f},
		{"__ret_t", VariantPresets.int_type_str},
		{"__ncall", Variant{FUNC, (NativeFunc_t)LIB_TIME_system_now_exec}}
	},
	VariantMode_constant
};
const Variant LIB_TIME_now {
	MAP,
	(MAP_t){
		{"__t", VariantPresets.obj_type_f},
		{"__ret_t", VariantPresets.int_type_str},
		{"__ncall", Variant{FUNC, (NativeFunc_t)LIB_TIME_now_exec}}
	},
	VariantMode_constant
};


const Variant LIB_TIME {
	MAP,
	(MAP_t){
		{"__name", Variant{STR, (STR_t)"TIME", VariantMode_constant}},
		{"__init__", LIB_TIME_init},
		{"system_now", LIB_TIME_system_now},
		{"now", LIB_TIME_now},
	}
};
