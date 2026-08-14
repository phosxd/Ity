#pragma once


void OP_Set_exec(ItyState& state, Variant*& first, Variant*& second, const OpSymbol& symbol, Variant& result, Variant*& result_ptr) {
	Variant* o1 = resovlve_potential_ref(state, first);
	Variant* o2 = resovlve_potential_ref(state, second);

	// Throw error if target variant is constant.
	if (o1->m == VariantMode_constant) {
		emit_error(ERR_cannot_change_constant);
		return;
	}

	Variant var;

	switch (symbol) {
		case OpSymbol_set:     {var = *o2;       break;}
		case OpSymbol_add_set: {var = *o1 + *o2; break;}
		case OpSymbol_sub_set: {var = *o1 - *o2; break;}
		case OpSymbol_mul_set: {var = *o1 * *o2; break;}
		case OpSymbol_div_set: {var = *o1 / *o2; break;}
		case OpSymbol_mod_set: {var = *o1 % *o2; break;}

		// Move second into first, unsetting the second variant.
		case OpSymbol_mov_set: {
			if (not o2->type_matches(*o1)) return;
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
		default: break;
	}

	// Throw error if types do not match & target variant's type is not dynamic.
	if (not var.type_matches(*o1)) return;

	var.m = o1->m; // Make sure the mode is kept in-tact.
	*o1 = std::move(var);
	result_ptr = first;
}




const auto OP_Set = new Operation{
	OP_Set_exec,
};
