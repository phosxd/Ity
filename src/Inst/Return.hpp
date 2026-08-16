#pragma once


static void INST_Return_processor(InstToken& token, const AnyMap_t& extra, const unsigned int& ln, const unsigned int& col) {
	bool found = false;
	std::vector<CompositeItem> reverse_nest = *AnyCast(std::vector<CompositeItem>*,extra.at("cn"));
	std::reverse(reverse_nest.begin(), reverse_nest.end());
	for (const CompositeItem& comp_item : reverse_nest) {
		if (comp_item.token.symbol != InstSymbol_func) continue;
		found = true;
		break;
	}

	// Throw error if there if instruciton is not under a function.
	if (not found) {
		emit_error(ERR_unexpected_inst, {token.args[0]}, ln,col);
		return;
	}
}




static void INST_Return_exec(ItyState& state, InstToken& token) {
	// Cleanly exit all scopes in the function.
	exit_ongoing_scopes(state.scope);

	// Get value from expression & set return value.
	const Variant* var = expr_exec(state, token.expr);
	state.scope.set_data("__R", var->t, *var, VariantMode_dynamic_type, HASHED_NAMES.__R);

	exec_jump_out = true;
}




const Instruction* INST_Return = new Instruction{
	.REQUIRED = 1,
	.exec = INST_Return_exec,
	.has_expr = true,
	.processor = INST_Return_processor
};
