#pragma once


static Variant LIB_TI_get_time(ItyState& _state, const ARR_t& args, const unsigned int mode) {
	if (not ExpectArgs(args, { {STR} })) return VPS.empty;

	const STR_t& precision = AnyCast(STR_t,args[0].d);
	const auto diff = Clock::now() - ((mode == 1) ? clock_start : Clock_t{});

	INT_t result = 0;
	if (precision == "us")      result = DurCast_us(diff).count();
	else if (precision == "ms") result = DurCast_ms(diff).count();
	else if (precision == "s")  result = DurCast_s(diff).count();
	else if (precision == "m")  result = DurCast_m(diff).count();
	else if (precision == "h")  result = DurCast_h(diff).count();
	else if (precision == "d")  result = DurCast_d(diff).count();
	else if (precision == "w")  result = DurCast_w(diff).count();
	else if (precision == "M")  result = DurCast_M(diff).count();
	else if (precision == "y")  result = DurCast_y(diff).count();
	return Variant{INT, result};
}


static Variant LIB_TI_snow(ItyState& state, const ARR_t& args) {return LIB_TI_get_time(state, args, 0);}
static Variant LIB_TI_now(ItyState& state, const ARR_t& args) {return LIB_TI_get_time(state, args, 1);}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_Time {
	MAP, (MAP_t){
		{"__name",      Variant{STR, (STR_t)"Time", VariantMode_constant}},
		{"__safe",      VPS.bool_true},
		{"snow",        NativeFuncTrans(INT,   (NativeFunc_t)LIB_TI_snow)},
		{"now",         NativeFuncTrans(INT,   (NativeFunc_t)LIB_TI_now)},
	},
	VariantMode_constant
};
