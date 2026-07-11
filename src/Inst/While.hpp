#pragma once


void INST_While_exec(const Instruction& inst, InstToken& token, ScopeState& state, const std::vector<std::string>& args) {
	const unsigned int args_len = args.size();
	std::string expr;
	expr.reserve(args_len);
	for (unsigned int i = 1; i < args_len; i++) {
		expr += ' '+args[i];
	}

	// Get value from expression.
	const Variant& value = expr_run(state, expr);

	// Throw error if not boolean.
	if (value.t != BOOL) {
		emit_error(ERR_expected_boolean_expression);
		return;
	}
	bool expr_passed = std::any_cast<bool>(value.d);

	// Jump past instructions in this composite if failed.
	if (not expr_passed) {exec_jump_value += token.composite_size;} // Add 1 to skip the end instruction, which for loops will jump back to it's corresponding composite instruction.
}


const Instruction INST_While {
	0,                // Required arg count.
	-1,               // Optional arg count.
	INST_While_exec,  // Function.
	true,             // Is composite.
};
