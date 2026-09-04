#pragma once


void OP_RefOps_exec(ItyState& state, Variant*& _first, Variant*& second, const OpSymbol& symbol, Variant& result, Variant*& result_ptr) {
	// Make pointer.
	if (symbol == OpSymbol_ptrref) result = Variant{PTR, second};

	// Dereference.
	else if (symbol == OpSymbol_deref) {
		if (second->t == PTR || second->t == REF) result_ptr = resovlve_potential_ref(state, second);
		// Emit error if invalid type to deref.
		else emit_error(ERR_cannot_dereference, {get_variant_type_name(second->t)});
	}
}




const auto OP_RefOps = new Operation{
	.exec = OP_RefOps_exec,
	.single_part = true,
};
