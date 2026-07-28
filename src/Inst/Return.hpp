#pragma once


void INST_Return_processor(InstToken& token, const AnyMap_t& extra, const unsigned int& ln, const unsigned int& col) {
	bool found = false;
	std::vector<CompositeItem> reverse_nest = *std::any_cast<std::vector<CompositeItem>*>(extra.at("composite_nest"));
	std::reverse(reverse_nest.begin(), reverse_nest.end());
	for (const CompositeItem& comp_item : reverse_nest) {
		if (comp_item.token.args[0] != "func") continue;
		found = true;
		break;
	}

	// Throw error if there if instruciton is not under a function.
	if (not found) {
		emit_error(ERR_unexpected_inst, {token.args[0]}, ln,col);
		return;
	}
}




void INST_Return_exec(ScopeState& state, InstToken& token) {
	// Cleanly exit all scopes in the function.
	exit_ongoing_scopes(state);

	// Get value from expression & set return value.
	const Variant* var = expr_exec(state, token.expr);
	set_data(state, "__RET__", var->t, var->d, VariantMode_dynamic_type);

	exec_jump_out = true;
}


const Instruction INST_Return {
	1,                 // Required arg count.
	INST_Return_exec,  // Function.
	false,             // Is composite.
	true,              // Has expression.
	INST_Return_processor,
};
