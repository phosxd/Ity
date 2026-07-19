#pragma once


void OP_Set_exec(ScopeState& state, Variant& first, Variant& second, const std::string& symbol, Variant& result, Variant*& result_ptr) {
	// Throw error if target variant is constant.
	if (first.m == VariantMode_constant) {
		emit_error(ERR_cannot_change_constant);
		return;
	}

	VariantData data;

	if (symbol == "=") data = second.d;
	else if (symbol == "+=") data = first.d + second.d;
	else if (symbol == "-=") data = first.d - second.d;
	else if (symbol == "*=") data = first.d * second.d;
	else if (symbol == "/=") data = first.d / second.d;
	else if (symbol == "%=") data = first.d % second.d;

	// Set to reference of second.
	// TODO: currently does not work as it is supposed to.
	//       at the moment, it just behaves like "=".
	else if (symbol == "@=") {
		if (not variant_data_type_matches(second.d, first)) return;
		first = second;
		result_ptr = &first;
		return;
	}


	// Throw error if types do not match & target variant's type is not dynamic.
	if (not variant_data_type_matches(data, first)) return;

	first.t = second.t;
	first.d = data;
	result_ptr = &first;
}


const Operation OP_Set {
	OP_Set_exec,
};
