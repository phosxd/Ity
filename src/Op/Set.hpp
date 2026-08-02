#pragma once


void OP_Set_exec(ScopeState& state, Variant*& first, Variant*& second, const std::string& symbol, Variant& result, Variant*& result_ptr) {
	Variant* o1 = resovlve_potential_ref(state, first);
	Variant* o2 = resovlve_potential_ref(state, second);

	// Throw error if target variant is constant.
	if (o1->m == VariantMode_constant) {
		emit_error(ERR_cannot_change_constant);
		return;
	}

	Variant var;

	if (symbol == "=")       var = *o2;
	else if (symbol == "+=") var = *o1 + *o2;
	else if (symbol == "-=") var = *o1 - *o2;
	else if (symbol == "*=") var = *o1 * *o2;
	else if (symbol == "/=") var = *o1 / *o2;
	else if (symbol == "%=") var = *o1 % *o2;

	// Move second into first, unsetting the second variant.
	else if (symbol == "<<=") {
		if (not variant_type_matches(*o2, *o1)) return;
		// Throw error if source variant is constant.
		if (second->m == VariantMode_constant) {
			emit_error(ERR_cannot_change_constant);
			return;
		}
		// Move second into first.
		*o1 = std::move(*o2);
		result_ptr = first;
		return;
	}

	// Throw error if types do not match & target variant's type is not dynamic.
	if (not variant_type_matches(var, *o1)) return;

	*o1 = var;
	result_ptr = first;
}


const Operation OP_Set {
	OP_Set_exec,
};
