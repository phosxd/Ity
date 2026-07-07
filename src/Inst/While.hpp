#pragma once


void INST_While_exec(const Instruction& inst, const InstToken& token, ScopeState& state, const std::vector<std::string>& args, const std::string& symbol) {
	const unsigned int args_len = args.size();
	std::string expr;
	expr.reserve(args_len);
	for (unsigned int i = 0; i < args_len; i++) {
		if (i == 0) {continue;}
		expr += ' '+args[i];
	}

	// Get value from expression.
	const Variant& value = expr_run(state, expr);

	// Throw error if not boolean.
	if (value.t != BOOL) {
		emit_error("Expected a boolean result in expression.");
		return;
	}
	bool expr_passed = std::get<bool>(value.d);

	// Jump past instructions in this composite if failed.
	if (not expr_passed) {exec_jump_value += token.composite_size;} // Add 1 to skip the end instruction, which for loops will jump back to it's corresponding composite instruction.
}


Instruction INST_While {
	0,
	-1,
	INST_While_exec,
	true, // Is composite.
};
