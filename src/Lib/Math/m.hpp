#pragma once

#include <cmath>


static Variant LIB_MT_abs(ItyState& _state, const ARR_t& args) {
	if (not ExpectArgs(args, { {INT,FLOAT} })) return VPS.empty;
	const Variant& var = args[0];
	if (var.t == INT) return Variant{INT, std::abs(AnyCast(int32_t,var.d))};
	return Variant{FLOAT, std::abs(AnyCast(double,var.d))};
}


static Variant LIB_MT_log(ItyState& _state, const ARR_t& args) {
	if (not ExpectArgs(args, { {INT,FLOAT} })) return VPS.empty;
	const Variant& var = args[0];
	if (var.t == INT) return Variant{INT, (INT_t)std::log(AnyCast(int32_t,var.d))};
	return Variant{FLOAT, (FLOAT_t)std::log(AnyCast(double,var.d))};
}


static Variant LIB_MT_sqrt(ItyState& _state, const ARR_t& args) {
	if (not ExpectArgs(args, { {INT,FLOAT} })) return VPS.empty;
	const Variant& var = args[0];
	if (var.t == INT) return Variant{INT, (INT_t)std::sqrt((double)AnyCast(int32_t,var.d))};
	return Variant{FLOAT, std::sqrtf(AnyCast(double,var.d))};
}


// Rounding functions.

static Variant LIB_MT_round(ItyState& state, const ARR_t& args) {
	if (not ExpectArgs(args, { {FLOAT} })) return VPS.empty;
	return Variant{INT, (INT_t)std::round(AnyCast(FLOAT_t,args[0].d))};
}
static Variant LIB_MT_floor(ItyState& state, const ARR_t& args) {
	if (not ExpectArgs(args, { {FLOAT} })) return VPS.empty;
	return Variant{INT, (INT_t)std::floor(AnyCast(FLOAT_t,args[0].d))};
}
static Variant LIB_MT_ceil(ItyState& state, const ARR_t& args) {
	if (not ExpectArgs(args, { {FLOAT} })) return VPS.empty;
	return Variant{INT, (INT_t)std::ceil(AnyCast(FLOAT_t,args[0].d))};
}


// Power.
static Variant LIB_MT_pow(ItyState& _state, const ARR_t& args) {
	if (not ExpectArgs(args, { {INT,FLOAT}, {INT,FLOAT} })) return VPS.empty;
	return Variant{
		FLOAT, (FLOAT_t)std::pow(
			args[0].to_float(),
			args[1].to_float()
		)
	};
}


// Sum.
static Variant LIB_MT_sum(ItyState& _state, const ARR_t& args) {
	FLOAT_t sum = 0.0;
	VariantType type = INT;
	for (size_t i = 0; i < args.size(); i++) {
		const Variant& arg = args[i];
		switch (arg.t) {
			case INT:   {sum += AnyCast(INT_t,arg.d);   break;}
			case FLOAT: {
				sum += AnyCast(FLOAT_t,arg.d);
				type = FLOAT;
				break;
			}
			default: expect_arg_types(arg, {INT,FLOAT}, i);
		}
	}
	if (type == INT) return Variant{INT, (INT_t)sum};
	else return Variant{FLOAT, (FLOAT_t)sum};
}


// Return a random number in range of `min` & `max` integer arguments.
static Variant LIB_MT_rand(ItyState& _state, const ARR_t& args) {
	if (not ExpectArgs(args, { {INT}, {INT} })) return VPS.empty;

	const INT_t& min = AnyCast(INT_t,args[0].d);
	const INT_t& max = AnyCast(INT_t,args[1].d);

	return Variant{INT, (INT_t)(std::rand() % (max-min+1) + min)};
}


// Return a random number in range of `min` & `max` integer arguments.
static Variant LIB_MT_set_seed(ItyState& _state, const ARR_t& args) {
	if (not ExpectArgs(args, { {INT} })) return VPS.empty;
	const INT_t& seed = AnyCast(INT_t,args[0].d);
	std::srand(seed);
	return VPS.empty;
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_Math {
	MAP, (MAP_t){
		{"__name",     Variant{STR, (STR_t)"Math", VariantMode_constant}},
		{"__safe",     VPS.bool_true},
		{"abs",        NativeFuncTrans(ANY,    (NativeFunc_t)LIB_MT_abs)},
		{"round",      NativeFuncTrans(INT,    (NativeFunc_t)LIB_MT_round)},
		{"floor",      NativeFuncTrans(INT,    (NativeFunc_t)LIB_MT_floor)},
		{"ceil",       NativeFuncTrans(INT,    (NativeFunc_t)LIB_MT_ceil)},
		{"log",        NativeFuncTrans(ANY,    (NativeFunc_t)LIB_MT_log)},
		{"sqrt",       NativeFuncTrans(ANY,    (NativeFunc_t)LIB_MT_sqrt)},
		{"pow",        NativeFuncTrans(FLOAT,  (NativeFunc_t)LIB_MT_pow)},
		{"sum",        NativeFuncTrans(ANY,    (NativeFunc_t)LIB_MT_sum)},
		{"rand",       NativeFuncTrans(INT,    (NativeFunc_t)LIB_MT_rand)},
		{"set_seed",   NativeFuncTrans(INT,    (NativeFunc_t)LIB_MT_set_seed)},
	},
	VariantMode_constant
};
