#pragma once


static void INST_Return_processor(InstToken& token, const InstProcArgs& args) {
	bool found = false;
	std::vector<CompositeItem> reverse_nest = *args.composite_nest;
	std::reverse(reverse_nest.begin(), reverse_nest.end());
	for (const CompositeItem& comp_item : reverse_nest) {
		if (comp_item.token.symbol != InstSymbol_func) continue;
		found = true;
		break;
	}

	// Throw error if there if instruciton is not under a function.
	if (not found) {
		emit_error(ERR_misplaced_inst, {token.args[0]}, args.ln, args.col);
		return;
	}
}




static void INST_Return_exec(ItyState& state, InstToken& token) {
	// Cleanly exit all scopes in the function.
	state.exit_ongoing_scopes(state.scope);
	// Set return value to expression result then jump out of the current execution.
	state.scope.set_data("__R", *expr_exec(state, token.expr), HASHED_NAMES.__R);
	state.exec_jump_out = true;
}




const Instruction* INST_Return = new Instruction{
	.REQUIRED = 1,
	.exec = INST_Return_exec,
	.has_expr = true,
	.clear_args = true,
	.processor = INST_Return_processor
};
