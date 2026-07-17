#pragma once


void INST_Return_exec(ScopeState& state, const Instruction* _inst, InstToken& token, const std::vector<std::string>& args) {
	// Cleanly exit all scopes in the function.
	exit_ongoing_scopes(state);

	// Get value from expression & set return value.
	const Variant& value = expr_run(state, token.expr);
	set_data(state, "__RET__", value.t, value.d, VariantMode_dynamic_type);

	exec_jump_out = true;
}


const Instruction INST_Return {
	1,                 // Required arg count.
	INST_Return_exec,  // Function.
	false,             // Is composite.
	true,              // Has expression.
};
