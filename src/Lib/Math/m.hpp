#pragma once

#include <cmath>


static Variant LIB_MT_math(ItyState& _state, const ARR_t& args, const std::string& func) {
	if (not expect_arg_count(args, 1) || not expect_arg_types(args[0], {INT,FLOAT}, 0)) return VariantPresets.none;
	const Variant& var = args[0];

	VariantType type;
	VariantData data;
	if (var.t == INT) {
		const INT_t& d = AnyCast(INT_t,var.d);
		type = INT;
		if (func == "abs")         data = std::abs(d);
		else if (func == "floor")  data = (INT_t)std::floor(d);
		else if (func == "ceil")   data = (INT_t)std::ceil(d);
		else if (func == "log")    data = (INT_t)std::log(d);
		else if (func == "sqrt")   data = (INT_t)std::sqrt((double)d);
	}
	else if (var.t == FLOAT) {
		const FLOAT_t& d = AnyCast(FLOAT_t,var.d);
		type = FLOAT;
		if (func == "abs")         data = (FLOAT_t)std::abs(d);
		else if (func == "floor")  data = (FLOAT_t)std::floor(d);
		else if (func == "ceil")   data = (FLOAT_t)std::ceil(d);
		else if (func == "log")    data = (FLOAT_t)std::log(d);
		else if (func == "sqrt")   data = (FLOAT_t)std::sqrtf(d);
	}

	return Variant{std::move(type), std::move(data)};
}


static Variant LIB_MT_abs   (ItyState& state, const ARR_t& args) {return LIB_MT_math(state, args, "abs");}
static Variant LIB_MT_floor (ItyState& state, const ARR_t& args) {return LIB_MT_math(state, args, "floor");}
static Variant LIB_MT_ceil  (ItyState& state, const ARR_t& args) {return LIB_MT_math(state, args, "ceil");}
static Variant LIB_MT_log   (ItyState& state, const ARR_t& args) {return LIB_MT_math(state, args, "log");}
static Variant LIB_MT_sqrt  (ItyState& state, const ARR_t& args) {return LIB_MT_math(state, args, "sqrt");}


static Variant LIB_MT_pow(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 2)
	|| not expect_arg_types(args[0], {INT,FLOAT}, 0)
	|| not expect_arg_types(args[1], {INT,FLOAT}, 1))
	return VariantPresets.none;

	return Variant{
		FLOAT, (FLOAT_t)std::pow(
			args[0].to_float(),
			args[1].to_float()
		)
	};
}


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
	if (not expect_arg_count(args, 2)
		|| not expect_arg_types(args[0], {INT}, 0)
		|| not expect_arg_types(args[1], {INT}, 1)
	) return VariantPresets.none;

	const INT_t& min = AnyCast(INT_t,args[0].d);
	const INT_t& max = AnyCast(INT_t,args[1].d);

	return Variant{INT, (INT_t)(std::rand() % (max-min+1) + min)};
}


// Return a random number in range of `min` & `max` integer arguments.
static Variant LIB_MT_set_seed(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1) || not expect_arg_types(args[0], {INT}, 0)) return VariantPresets.none;

	const INT_t& seed = AnyCast(INT_t,args[0].d);
	std::srand(seed);

	return VariantPresets.none;
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_Math {
	MAP, (MAP_t){
		{"__name",     Variant{STR, (STR_t)"Math", VariantMode_constant}},
		{"__safe",     Variant{BOOL, true}},
		{"abs",        NativeFuncTrans(ANY,    (NativeFunc_t)LIB_MT_abs)},
		{"floor",      NativeFuncTrans(ANY,    (NativeFunc_t)LIB_MT_floor)},
		{"ceil",       NativeFuncTrans(ANY,    (NativeFunc_t)LIB_MT_ceil)},
		{"log",        NativeFuncTrans(ANY,    (NativeFunc_t)LIB_MT_log)},
		{"sqrt",       NativeFuncTrans(ANY,    (NativeFunc_t)LIB_MT_sqrt)},
		{"pow",        NativeFuncTrans(FLOAT,  (NativeFunc_t)LIB_MT_pow)},
		{"sum",        NativeFuncTrans(ANY,    (NativeFunc_t)LIB_MT_sum)},
		{"rand",       NativeFuncTrans(INT,    (NativeFunc_t)LIB_MT_rand)},
		{"set_seed",   NativeFuncTrans(INT,    (NativeFunc_t)LIB_MT_set_seed)},
	},
	VariantMode_constant
};
