#pragma once
// Fragment Size: 5342

#include <cmath>


static Variant LIB_MT_abs(ItyState& _state, ARR_t& args) {
	const Variant& v = args[0];
	return (v.t == VT_INT64) ? Variant{v.t, (int64_t)std::abs(AnyCast(int64_t,v.d))}
	: Variant{v.t, (FLOAT64_t)std::abs(AnyCast(double,v.d))};
}


static Variant LIB_MT_log(ItyState& _state, ARR_t& args) {
	const Variant& v = args[0];
	return (v.t == VT_INT64) ? Variant{v.t, (int64_t)std::log(AnyCast(int64_t,v.d))}
	: Variant{v.t, (FLOAT64_t)std::log(AnyCast(double,v.d))};
}


static Variant LIB_MT_sqrt(ItyState& _state, ARR_t& args) {
	const Variant& v = args[0];
	return (v.t == VT_INT64) ? Variant{v.t, (int64_t)std::sqrt((double)AnyCast(int64_t,v.d))}
	: Variant{v.t, (FLOAT64_t)std::sqrtf((float)AnyCast(double,v.d))};
}


// Rounding functions.

static Variant LIB_MT_round(ItyState& state, ARR_t& args) {
	return Variant{VT_INT64, (int64_t)std::round(AnyCast(FLOAT64_t,args[0].d))};
}
static Variant LIB_MT_floor(ItyState& state, ARR_t& args) {
	if (not ExpectArgs(args, { {VT_FLOAT64} })) return VPS.none;
	return Variant{VT_INT64, (int64_t)std::floor(AnyCast(FLOAT64_t,args[0].d))};
}
static Variant LIB_MT_ceil(ItyState& state, ARR_t& args) {
	if (not ExpectArgs(args, { {VT_FLOAT64} })) return VPS.none;
	return Variant{VT_INT64, (int64_t)std::ceil(AnyCast(FLOAT64_t,args[0].d))};
}


// Power.
static Variant LIB_MT_pow(ItyState& _state, ARR_t& args) {
	return Variant{
		VT_FLOAT64, (FLOAT64_t)std::pow(
			args[0].to_float64(),
			args[1].to_float64()
		)
	};
}


// Sum.
static Variant LIB_MT_sum(ItyState& _state, ARR_t& args) {
	FLOAT64_t sum = 0.0;
	for (size_t i = 0; i < args.size(); i++) {
		const Variant& arg = args[i];
		if (not arg.is_number()) {
			expect_arg_types(arg, {VT_INT64,VT_FLOAT64}, i);
			continue;
		}
		sum += arg.to_float64();
	}
	return Variant{VT_FLOAT64, sum};
}


// Precise linear interpolation.
static Variant LIB_MT_lerp(ItyState& _state, ARR_t& args) {
	const FLOAT64_t& b = AnyCast(FLOAT64_t,args[1].d);
	const FLOAT64_t& t = AnyCast(FLOAT64_t,args[2].d);
	return Variant{VT_FLOAT64, AnyCast(FLOAT64_t,args[0].d)*(1.0-t)+(b*t)};
}


// Precise value range remap.
static Variant LIB_MT_remap(ItyState& state, ARR_t& args) {
	// Origin range.
	const FLOAT64_t& o_a = AnyCast(FLOAT64_t,args[0].d);
	const FLOAT64_t& o_b = AnyCast(FLOAT64_t,args[1].d);
	// Target range.
	const FLOAT64_t& t_a = AnyCast(FLOAT64_t,args[2].d);
	const FLOAT64_t& t_b = AnyCast(FLOAT64_t,args[3].d);
	// Value to translate.
	const FLOAT64_t& v = AnyCast(FLOAT64_t,args[4].d);

	// My own formula for linear remapping, that I wrote back in early 2025.
	// May or may not have issues, but as far as I have tested it seems to work nicely.
	return Variant{VT_FLOAT64, t_a+(((v-o_a)/(o_b-o_a))*(t_b-t_a))};
}


// Return a random number in range of `min` & `max` integer arguments.
static Variant LIB_MT_rand(ItyState& _state, ARR_t& args) {
	const int64_t& min = AnyCast(int64_t,args[0].d);
	const int64_t& max = AnyCast(int64_t,args[1].d);

	return Variant{VT_INT64, (int64_t)(std::rand() % (max-min+1) + min)};
}


// Return a random number in range of `min` & `max` integer arguments.
static Variant LIB_MT_set_seed(ItyState& _state, ARR_t& args) {
	const int64_t& seed = AnyCast(int64_t,args[0].d);
	std::srand((unsigned int)seed);
	return VPS.none;
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_Math {
	VT_MAP, (MAP_t){
		{"__name",     Variant{VT_STR, (STR_t)"Math", VM_const}},
		{"__safe",     VPS.bool_true},
		{"abs",        NativeFuncTrans(VT_ANY,      LIB_MT_abs,      { {VT_INT64,VT_FLOAT64} })},
		{"round",      NativeFuncTrans(VT_INT64,    LIB_MT_round,    { {VT_FLOAT64} })},
		{"floor",      NativeFuncTrans(VT_INT64,    LIB_MT_floor,    { {VT_FLOAT64} })},
		{"ceil",       NativeFuncTrans(VT_INT64,    LIB_MT_ceil,     { {VT_FLOAT64} })},
		{"log",        NativeFuncTrans(VT_ANY,      LIB_MT_log,      { {VT_INT64,VT_FLOAT64} })},
		{"sqrt",       NativeFuncTrans(VT_ANY,      LIB_MT_sqrt,     { {VT_INT64,VT_FLOAT64} })},
		{"pow",        NativeFuncTrans(VT_FLOAT64,  LIB_MT_pow,      { {VT_INT64,VT_FLOAT64}, {VT_INT64,VT_FLOAT64} })},
		{"sum",        NativeFuncTrans(VT_FLOAT64,  LIB_MT_sum,      {})},
		{"lerp",       NativeFuncTrans(VT_FLOAT64,  LIB_MT_lerp,     { {VT_FLOAT64}, {VT_FLOAT64}, {VT_FLOAT64} })},
		{"remap",      NativeFuncTrans(VT_FLOAT64,  LIB_MT_remap,    { {VT_FLOAT64}, {VT_FLOAT64}, {VT_FLOAT64}, {VT_FLOAT64}, {VT_FLOAT64} })},
		{"rand",       NativeFuncTrans(VT_INT64,    LIB_MT_rand,     { {VT_INT64}, {VT_INT64} })},
		{"set_seed",   NativeFuncTrans(VT_NONE,     LIB_MT_set_seed, { {VT_INT64} })},
	},
	VM_const
};
