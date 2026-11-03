#pragma once


static Variant LIB_TI_get_time(ItyState& _state, const ARR_t& args, const uint8_t mode) {
	const STR_t& precision = AnyCast(STR_t,args[0].d);
	const auto diff = Clock::now() - ((mode == 1) ? clock_start : Clock_t{});

	uint64_t result = 0;
	if (precision == "us")      result = DurCast_us(diff).count();
	else if (precision == "ms") result = DurCast_ms(diff).count();
	else if (precision == "s")  result = DurCast_s(diff).count();
	else if (precision == "m")  result = DurCast_m(diff).count();
	else if (precision == "h")  result = DurCast_h(diff).count();
	else if (precision == "d")  result = DurCast_d(diff).count();
	else if (precision == "w")  result = DurCast_w(diff).count();
	else if (precision == "M")  result = DurCast_M(diff).count();
	else if (precision == "y")  result = DurCast_y(diff).count();
	return Variant{VT_UINT64, result};
}


static Variant LIB_TI_snow(ItyState& state, ARR_t& args) {return LIB_TI_get_time(state, args, 0);}
static Variant LIB_TI_now(ItyState& state, ARR_t& args) {return LIB_TI_get_time(state, args, 1);}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_Time {
	VT_MAP, (MAP_t){
		{"__name",      Variant{VT_STR, (STR_t)"Time", VM_const}},
		{"__safe",      VPS.bool_true},
		{"snow",        NativeFuncTrans(VT_UINT64,   LIB_TI_snow,  { {VT_STR} })},
		{"now",         NativeFuncTrans(VT_UINT64,   LIB_TI_now,   { {VT_STR} })},
	},
	VM_const
};
