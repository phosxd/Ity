#pragma once

#include <cmath>


Variant LIB_MATH_init(const ARR_t& args) {
	return VariantPresets.empty;
}


Variant LIB_MATH_math(const ARR_t& args, const std::string& func) {
	if (not expect_arg_count(args, 1)) return VariantPresets.empty;
	const Variant& var = args[0];

	VariantType type;
	VariantData data;
	if (var.t == INT) {
		const int& d = std::any_cast<int>(var.d);
		type = INT;
		if (func == "abs") data = std::abs(d);
		else if (func == "floor") data = std::floor(d);
		else if (func == "ceil") data = std::ceil(d);
		else if (func == "log") data = (int)std::log(d);
		else if (func == "sqrt") data = (int)std::sqrt((double)d);
	}
	else if (var.t == FLOAT) {
		const float& d = std::any_cast<float>(var.d);
		type = FLOAT;
		if (func == "abs") data = std::abs(d);
		else if (func == "floor") data = std::floor(d);
		else if (func == "ceil") data = std::ceil(d);
		else if (func == "log") data = (float)std::log(d);
		else if (func == "sqrt") data = std::sqrtf(d);
	}
	else {
		emit_error(ERR_invalid_func_arg_type, {"0", "INT or FLOAT", get_variant_type_name(var.t)});
		return VariantPresets.empty;
	}

	return Variant{type, std::move(data)};
}


Variant LIB_MATH_abs(const ARR_t& args) {return LIB_MATH_math(args, "abs");}
Variant LIB_MATH_floor(const ARR_t& args) {return LIB_MATH_math(args, "floor");}
Variant LIB_MATH_ceil(const ARR_t& args) {return LIB_MATH_math(args, "ceil");}
Variant LIB_MATH_log(const ARR_t& args) {return LIB_MATH_math(args, "log");}
Variant LIB_MATH_sqrt(const ARR_t& args) {return LIB_MATH_math(args, "sqrt");}


Variant LIB_MATH_pow(const ARR_t& args) {
	if (not expect_arg_count(args, 2)) return VariantPresets.empty;
	const std::vector<VariantType> valid_types = {INT, FLOAT};
	if (not expect_arg_types(args[0], valid_types, 0) || not expect_arg_types(args[1], valid_types, 1)) return VariantPresets.empty;

	return Variant{
		FLOAT,
		(float)std::pow(
			var_to_float(args[0]),
			var_to_float(args[1])
		)
	};
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_MATH {
	MAP,
	(MAP_t){
		{"__name",     Variant{STR, (STR_t)"MATH", VariantMode_constant}},
		{"__init__",   NativeFuncTrans(VariantPresets.none_type_str,   (NativeFunc_t)LIB_MATH_init)},
		{"abs",        NativeFuncTrans(VariantPresets.any_type_str,    (NativeFunc_t)LIB_MATH_abs)},
		{"floor",      NativeFuncTrans(VariantPresets.any_type_str,    (NativeFunc_t)LIB_MATH_floor)},
		{"ceil",       NativeFuncTrans(VariantPresets.any_type_str,    (NativeFunc_t)LIB_MATH_ceil)},
		{"log",        NativeFuncTrans(VariantPresets.any_type_str,    (NativeFunc_t)LIB_MATH_log)},
		{"sqrt",       NativeFuncTrans(VariantPresets.any_type_str,    (NativeFunc_t)LIB_MATH_sqrt)},
		{"pow",        NativeFuncTrans(VariantPresets.float_type_str,  (NativeFunc_t)LIB_MATH_pow)},
	},
	VariantMode_constant
};
