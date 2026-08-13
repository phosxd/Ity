#pragma once


Variant LIB_Time_get_time(ItyState& _state, const ARR_t& args, const unsigned int mode) {
	if (not expect_arg_count(args, 1) || not expect_arg_types(args[0], {STR}, 0)) return VariantPresets.none;

	const STR_t& precision = AnyCast(STR_t,args[0].d);

	// Initialize clocks.
	const Clock_t& clock_1 = Clock::now();
	std::chrono::time_point<std::chrono::high_resolution_clock> clock_2;
	if (mode == 1) clock_2 = clock_start;

	// Get value.
	INT_t result = 0;
	if (precision == "us")      result = DurCast_us(clock_1-clock_2).count();
	else if (precision == "ms") result = DurCast_ms(clock_1-clock_2).count();
	else if (precision == "s")  result = DurCast_s(clock_1-clock_2).count();
	else if (precision == "m")  result = DurCast_m(clock_1-clock_2).count();
	else if (precision == "h")  result = DurCast_h(clock_1-clock_2).count();
	else if (precision == "d")  result = DurCast_d(clock_1-clock_2).count();
	else if (precision == "w")  result = DurCast_w(clock_1-clock_2).count();
	else if (precision == "M")  result = DurCast_M(clock_1-clock_2).count();
	else if (precision == "y")  result = DurCast_y(clock_1-clock_2).count();

	return Variant{INT, std::move(result)};
}


Variant LIB_Time_snow(ItyState& state, const ARR_t& args) {
	return LIB_Time_get_time(state, args, 0);
}


Variant LIB_Time_now(ItyState& state, const ARR_t& args) {
	return LIB_Time_get_time(state, args, 1);
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_Time {
	MAP, (MAP_t){
		{"__name",      Variant{STR, (STR_t)"Time", VariantMode_constant}},
		{"snow",        NativeFuncTrans(INT,   (NativeFunc_t)LIB_Time_snow)},
		{"now",         NativeFuncTrans(INT,   (NativeFunc_t)LIB_Time_now)},
	},
	VariantMode_constant
};
