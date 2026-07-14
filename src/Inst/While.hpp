#pragma once


void INST_While_exec(const Instruction* _inst, InstToken& token, ScopeState& state, const std::vector<std::string>& args) {
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
	if (not expr_passed) {
		exec_jump_value += token.composite_size; // Add 1 to skip the end instruction, otherwise will jump back to this instruction.
		// Scope out if previously scoped in.
		if (token.meta.size() > 0) {
			scope_out(state);
			token.meta.clear();
			scoped_loop_tokens.pop_back();
		}
	}
	// If entering loop for first time, scope in.
	else if (token.meta.size() == 0) {
		token.meta = {true};
		scope_in(state);
		scoped_loop_tokens.push_back(&token);
	}
}


const Instruction INST_While {
	0,                // Required arg count.
	-1,               // Optional arg count.
	INST_While_exec,  // Function.
	true,             // Is composite.
};
