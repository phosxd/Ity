#pragma once

#include <cmath>


Variant LIB_Math_init(ScopeState& _state, const ARR_t& args) {
	return VariantPresets.empty;
}


Variant LIB_Math_math(ScopeState& _state, const ARR_t& args, const std::string& func) {
	if (not expect_arg_count(args, 1)) return VariantPresets.empty;
	const Variant& var = args[0];

	VariantType type;
	VariantData data;
	if (var.t == INT) {
		const INT_t& d = AnyCast(INT_t,var.d);
		type = INT;
		if (func == "abs") data = std::abs(d);
		else if (func == "floor")  data = (INT_t)std::floor(d);
		else if (func == "ceil")   data = (INT_t)std::ceil(d);
		else if (func == "log")    data = (INT_t)std::log(d);
		else if (func == "sqrt")   data = (INT_t)std::sqrt((double)d);
	}
	else if (var.t == FLOAT) {
		const FLOAT_t& d = std::any_cast<const FLOAT_t&>(var.d);
		type = FLOAT;
		if (func == "abs")         data = (FLOAT_t)std::abs(d);
		else if (func == "floor")  data = (FLOAT_t)std::floor(d);
		else if (func == "ceil")   data = (FLOAT_t)std::ceil(d);
		else if (func == "log")    data = (FLOAT_t)std::log(d);
		else if (func == "sqrt")   data = (FLOAT_t)std::sqrtf(d);
	}
	else {
		emit_error(ERR_invalid_func_arg_type, {"0", "INT or FLOAT", get_variant_type_name(var.t)});
		return VariantPresets.empty;
	}

	return Variant{type, std::move(data)};
}


Variant LIB_Math_abs(ScopeState& state, const ARR_t& args) {return LIB_Math_math(state, args, "abs");}
Variant LIB_Math_floor(ScopeState& state, const ARR_t& args) {return LIB_Math_math(state, args, "floor");}
Variant LIB_Math_ceil(ScopeState& state, const ARR_t& args) {return LIB_Math_math(state, args, "ceil");}
Variant LIB_Math_log(ScopeState& state, const ARR_t& args) {return LIB_Math_math(state, args, "log");}
Variant LIB_Math_sqrt(ScopeState& state, const ARR_t& args) {return LIB_Math_math(state, args, "sqrt");}


Variant LIB_Math_pow(ScopeState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 2)) return VariantPresets.empty;
	const std::vector<VariantType> valid_types = {INT, FLOAT};
	if (not expect_arg_types(args[0], valid_types, 0) || not expect_arg_types(args[1], valid_types, 1)) return VariantPresets.empty;

	return Variant{
		FLOAT, (FLOAT_t)std::pow(
			var_to_float(args[0]),
			var_to_float(args[1])
		)
	};
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_Math {
	MAP, (MAP_t){
		{"__name",     Variant{STR, (STR_t)"Math", VariantMode_constant}},
		{"__init__",   NativeFuncTrans(NONE,   (NativeFunc_t)LIB_Math_init)},
		{"abs",        NativeFuncTrans(ANY,    (NativeFunc_t)LIB_Math_abs)},
		{"floor",      NativeFuncTrans(ANY,    (NativeFunc_t)LIB_Math_floor)},
		{"ceil",       NativeFuncTrans(ANY,    (NativeFunc_t)LIB_Math_ceil)},
		{"log",        NativeFuncTrans(ANY,    (NativeFunc_t)LIB_Math_log)},
		{"sqrt",       NativeFuncTrans(ANY,    (NativeFunc_t)LIB_Math_sqrt)},
		{"pow",        NativeFuncTrans(FLOAT,  (NativeFunc_t)LIB_Math_pow)},
	},
	VariantMode_constant
};
