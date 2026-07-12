#pragma once


void INST_Return_exec(const Instruction& _inst, InstToken& token, ScopeState& state, const std::vector<std::string>& args) {
	const unsigned int args_len = args.size();
	std::string expr;
	expr.reserve(args_len);
	for (unsigned int i = 1; i < args_len; i++) {
		expr += ' '+args[i];
	}

	// Get value from expression & set return value.
	const Variant& value = expr_run(state, expr);
	set_data(state, "__RET__", value.t, value.d, VariantMode_dynamic_type);

	exec_jump_out = true;
}


const Instruction INST_Return {
	0,                 // Required arg count.
	-1,                // Optional arg count.
	INST_Return_exec,  // Function.
	false,             // Is composite.
};
