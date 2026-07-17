#pragma once


Variant LIB_Time_init(ScopeState& _state, const ARR_t& args) {
	return VariantPresets.empty;
}


Variant LIB_Time_get_time(ScopeState& _state, const ARR_t& args, const unsigned int mode) {
	if (not expect_arg_count(args, 1) || not expect_arg_types(args[0], {STR}, 0)) return VariantPresets.empty;

	const STR_t& precision = std::any_cast<STR_t>(args[0].d);

	// Initialize clocks.
	const Clock_t& clock_1 = Clock::now();
	std::chrono::time_point<std::chrono::high_resolution_clock> clock_2;
	if (mode == 1) clock_2 = clock_start;

	// Get value.
	int result = 0;
	if (precision == "us") {
		result = std::chrono::duration_cast<std::chrono::microseconds>(clock_1-clock_2).count();
	}
	else if (precision == "ms") result = std::chrono::duration_cast<std::chrono::milliseconds>(clock_1-clock_2).count();
	else if (precision == "s") result = std::chrono::duration_cast<std::chrono::seconds>(clock_1-clock_2).count();
	else if (precision == "m") result = std::chrono::duration_cast<std::chrono::minutes>(clock_1-clock_2).count();
	else if (precision == "h") result = std::chrono::duration_cast<std::chrono::hours>(clock_1-clock_2).count();
	else if (precision == "d") result = std::chrono::duration_cast<std::chrono::days>(clock_1-clock_2).count();
	else if (precision == "w") result = std::chrono::duration_cast<std::chrono::weeks>(clock_1-clock_2).count();
	else if (precision == "M") result = std::chrono::duration_cast<std::chrono::months>(clock_1-clock_2).count();
	else if (precision == "y") result = std::chrono::duration_cast<std::chrono::years>(clock_1-clock_2).count();

	return Variant{INT, std::move(result)};
}


Variant LIB_Time_system_now(ScopeState& state, const ARR_t& args) {
	return LIB_Time_get_time(state, args, 0);
}


Variant LIB_Time_now(ScopeState& state, const ARR_t& args) {
	return LIB_Time_get_time(state, args, 1);
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_Time {
	MAP,
	(MAP_t){
		{"__name", Variant{STR, (STR_t)"Time", VariantMode_constant}},
		{"__init__", NativeFuncTrans(VariantPresets.none_type_str, (NativeFunc_t)LIB_Time_init)},
		{"system_now", NativeFuncTrans(VariantPresets.int_type_str, (NativeFunc_t)LIB_Time_system_now)},
		{"now", NativeFuncTrans(VariantPresets.int_type_str, (NativeFunc_t)LIB_Time_now)},
	},
	VariantMode_constant
};
