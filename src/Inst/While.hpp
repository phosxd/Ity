#pragma once


void INST_While_exec(ScopeState& state, const Instruction* _inst, InstToken& token, const std::vector<std::string>& args) {
	// Get value from expression.
	const Variant& value = expr_exec(state, token.expr);

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
		if (token.declarative_composite && token.meta.size() > 0) {
			scope_out(state);
			token.meta.clear();
			scoped_tokens.pop_back();
		}
	}
	// If entering loop for first time & the composite is declarative, then scope in.
	else if (token.declarative_composite && token.meta.size() == 0) {
		token.meta = {true};
		scope_in(state);
		scoped_tokens.push_back(&token);
	}
}


const Instruction INST_While {
	1,                // Required arg count.
	INST_While_exec,  // Function.
	true,             // Is composite.
	true,             // Has expression.
};
