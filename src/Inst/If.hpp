#pragma once


void INST_If_exec(const Instruction& inst, const InstToken& token, ScopeState& state, const std::vector<std::string>& args, const std::string& symbol) {
	const unsigned int args_len = args.size();
	std::string expr;
	expr.reserve(args_len);
	for (unsigned int i = 0; i < args_len; i++) {
		if (i == 0) {continue;}
		expr += ' '+args[i];
	}

	// Get value from expression.
	Variant value = expr_run(state, expr);

	// Throw error if not boolean.
	if (value.t != BOOL) {
		emit_error("Expected a boolean result in expression.");
		return;
	}

	// Jump past instructions under this composite if condition failed.
	bool can_continue = std::get<bool>(value.d);
	if (can_continue == false) {
		exec_jump_value += token.composite_size;
		return;
	}
}


Instruction INST_If {
	1, // "REQUIRED". Jump Count.
	0,
	INST_If_exec,
	true, // Is composite.
};
