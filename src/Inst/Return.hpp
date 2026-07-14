#pragma once


void INST_Return_exec(const Instruction* _inst, InstToken& token, const std::vector<std::string>& args) {
	const unsigned int args_len = args.size();
	std::string expr;
	expr.reserve(args_len);
	for (unsigned int i = 1; i < args_len; i++) {
		expr += ' '+args[i];
	}

	// Cleanly exit all scopes in the function.
	exit_ongoing_scopes();

	// Get value from expression & set return value.
	const Variant& value = expr_run(expr);
	set_data(ST, "__RET__", value.t, value.d, VariantMode_dynamic_type);

	exec_jump_out = true;
}


const Instruction INST_Return {
	0,                 // Required arg count.
	-1,                // Optional arg count.
	INST_Return_exec,  // Function.
	false,             // Is composite.
};
