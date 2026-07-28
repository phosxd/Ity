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

	// Move second into first, unsetting the second variant.
	else if (symbol == "<<=") {
		if (not variant_data_type_matches(second.d, first)) return;
		// Throw error if source variant is constant.
		if (second.m == VariantMode_constant) {
			emit_error(ERR_cannot_change_constant);
			return;
		}
		// Move second into first.
		first = std::move(second);
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
