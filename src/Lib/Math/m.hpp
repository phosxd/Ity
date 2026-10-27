#pragma once
// Fragment Size: 5342

#include <cmath>


static Variant LIB_MT_abs(ItyState& _state, ARR_t& args) {
	if (not ExpectArgs(args, { {VT_INT,VT_FLOAT} })) return VPS.none;
	const Variant& v = args[0];
	if (v.t == VT_INT) return Variant{v.t, (INT_t)std::abs(AnyCast(INT_t,v.d))};
	return Variant{v.t, (FLOAT_t)std::abs(AnyCast(double,v.d))};
}


static Variant LIB_MT_log(ItyState& _state, ARR_t& args) {
	if (not ExpectArgs(args, { {VT_INT,VT_FLOAT} })) return VPS.none;
	const Variant& v = args[0];
	if (v.t == VT_INT) return Variant{v.t, (INT_t)std::log(AnyCast(INT_t,v.d))};
	return Variant{v.t, (FLOAT_t)std::log(AnyCast(double,v.d))};
}


static Variant LIB_MT_sqrt(ItyState& _state, ARR_t& args) {
	if (not ExpectArgs(args, { {VT_INT,VT_FLOAT} })) return VPS.none;
	const Variant& v = args[0];
	if (v.t == VT_INT) return Variant{v.t, (INT_t)std::sqrt((double)AnyCast(INT_t,v.d))};
	return Variant{v.t, (FLOAT_t)std::sqrtf((float)AnyCast(double,v.d))};
}


// Rounding functions.

static Variant LIB_MT_round(ItyState& state, ARR_t& args) {
	if (not ExpectArgs(args, { {VT_FLOAT} })) return VPS.none;
	return Variant{VT_INT, (INT_t)std::round(AnyCast(FLOAT_t,args[0].d))};
}
static Variant LIB_MT_floor(ItyState& state, ARR_t& args) {
	if (not ExpectArgs(args, { {VT_FLOAT} })) return VPS.none;
	return Variant{VT_INT, (INT_t)std::floor(AnyCast(FLOAT_t,args[0].d))};
}
static Variant LIB_MT_ceil(ItyState& state, ARR_t& args) {
	if (not ExpectArgs(args, { {VT_FLOAT} })) return VPS.none;
	return Variant{VT_INT, (INT_t)std::ceil(AnyCast(FLOAT_t,args[0].d))};
}


// Power.
static Variant LIB_MT_pow(ItyState& _state, ARR_t& args) {
	if (not ExpectArgs(args, { {VT_INT,VT_FLOAT}, {VT_INT,VT_FLOAT} })) return VPS.none;
	return Variant{
		VT_FLOAT, (FLOAT_t)std::pow(
			args[0].to_float(),
			args[1].to_float()
		)
	};
}


// Sum.
static Variant LIB_MT_sum(ItyState& _state, ARR_t& args) {
	FLOAT_t sum = 0.0;
	VariantType type = VT_INT;
	for (size_t i = 0; i < args.size(); i++) {
		const Variant& arg = args[i];
		switch (arg.t) {
			case VT_INT:   {sum += (FLOAT_t)AnyCast(INT_t,arg.d);   break;}
			case VT_FLOAT: {
				sum += AnyCast(FLOAT_t,arg.d);
				type = VT_FLOAT;
				break;
			}
			default: expect_arg_types(arg, {VT_INT,VT_FLOAT}, i);
		}
	}
	if (type == VT_INT) return Variant{VT_INT, (INT_t)sum};
	else return Variant{VT_FLOAT, sum};
}


// Precise linear interpolation.
static Variant LIB_MT_lerp(ItyState& _state, ARR_t& args) {
	if (not ExpectArgs(args, { {VT_FLOAT}, {VT_FLOAT}, {VT_FLOAT} })) return VPS.none;
	const FLOAT_t& b = AnyCast(FLOAT_t,args[1].d);
	const FLOAT_t& t = AnyCast(FLOAT_t,args[2].d);
	return Variant{VT_FLOAT, AnyCast(FLOAT_t,args[0].d)*(1.0-t)+(b*t)};
}


// Precise value range remap.
static Variant LIB_MT_remap(ItyState& state, ARR_t& args) {
	if (not ExpectArgs(args, { {VT_FLOAT}, {VT_FLOAT}, {VT_FLOAT}, {VT_FLOAT}, {VT_FLOAT} })) return VPS.none;
	// Origin range.
	const FLOAT_t& o_a = AnyCast(FLOAT_t,args[0].d);
	const FLOAT_t& o_b = AnyCast(FLOAT_t,args[1].d);
	// Target range.
	const FLOAT_t& t_a = AnyCast(FLOAT_t,args[2].d);
	const FLOAT_t& t_b = AnyCast(FLOAT_t,args[3].d);
	// Value to translate.
	const FLOAT_t& v = AnyCast(FLOAT_t,args[4].d);

	// My own formula for linear remapping, that I wrote back in early 2025.
	// May or may not have issues, but as far as I have tested it seems to work nicely.
	return Variant{VT_FLOAT, t_a+(((v-o_a)/(o_b-o_a))*(t_b-t_a))};
}


// Return a random number in range of `min` & `max` integer arguments.
static Variant LIB_MT_rand(ItyState& _state, ARR_t& args) {
	if (not ExpectArgs(args, { {VT_INT}, {VT_INT} })) return VPS.none;

	const INT_t& min = AnyCast(INT_t,args[0].d);
	const INT_t& max = AnyCast(INT_t,args[1].d);

	return Variant{VT_INT, (INT_t)(std::rand() % (max-min+1) + min)};
}


// Return a random number in range of `min` & `max` integer arguments.
static Variant LIB_MT_set_seed(ItyState& _state, ARR_t& args) {
	if (not ExpectArgs(args, { {VT_INT} })) return VPS.none;
	const INT_t& seed = AnyCast(INT_t,args[0].d);
	std::srand((unsigned int)seed);
	return VPS.none;
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_Math {
	VT_MAP, (MAP_t){
		{"__name",     Variant{VT_STR, (STR_t)"Math", VariantMode_constant}},
		{"__safe",     VPS.bool_true},
		{"abs",        NativeFuncTrans(VT_ANY,    LIB_MT_abs)},
		{"round",      NativeFuncTrans(VT_INT,    LIB_MT_round)},
		{"floor",      NativeFuncTrans(VT_INT,    LIB_MT_floor)},
		{"ceil",       NativeFuncTrans(VT_INT,    LIB_MT_ceil)},
		{"log",        NativeFuncTrans(VT_ANY,    LIB_MT_log)},
		{"sqrt",       NativeFuncTrans(VT_ANY,    LIB_MT_sqrt)},
		{"pow",        NativeFuncTrans(VT_FLOAT,  LIB_MT_pow)},
		{"sum",        NativeFuncTrans(VT_ANY,    LIB_MT_sum)},
		{"lerp",       NativeFuncTrans(VT_FLOAT,  LIB_MT_lerp)},
		{"remap",      NativeFuncTrans(VT_FLOAT,  LIB_MT_remap)},
		{"rand",       NativeFuncTrans(VT_INT,    LIB_MT_rand)},
		{"set_seed",   NativeFuncTrans(VT_INT,    LIB_MT_set_seed)},
	},
	VariantMode_constant
};
