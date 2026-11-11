#pragma once


void OP_RefOps_pre_exec(ItyState& state, ExprState& expr_state, Variant*& first, const OpSymbol& symbol, bool& _eval_second_operand, Variant& _result, Variant*& _result_ptr) {
	if (symbol == OpSymbol_ref) expr_state.record_path = true;
}


void OP_RefOps_exec(ItyState& state, ExprState& expr_state, Variant*& _first, Variant*& second, const OpSymbol& symbol, Variant& result, Variant*& result_ptr) {
	// Make reference.
	if (symbol == OpSymbol_ref) {
		result = Variant{VT_REF, expr_state.path};
		expr_state.record_path = false;
	}

	// Dereference.
	else if (symbol == OpSymbol_deref) {
		if (second->t == VT_REF) result_ptr = resovlve_potential_ref(state, second);
		// Emit error if invalid type to deref.
		else emit_error(ERR_cannot_dereference, {get_variant_type_name(second->t)});
	}
}




const auto OP_RefOps = new Operation{
	.exec = OP_RefOps_exec,
	.pre_exec = OP_RefOps_pre_exec,
	.unary = true,
};
