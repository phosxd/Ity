#pragma once


void INST_If_exec(const Instruction& _inst, InstToken& token, ScopeState& state, const std::vector<std::string>& args) {
	const unsigned int args_len = args.size();
	const std::string symbol = args[0];
	std::string expr;
	expr.reserve(args_len);
	for (unsigned int i = 1; i < args_len; i++) {
		expr += ' '+args[i];
	}

	// Get value from expression.
	const Variant& value = expr_run(state, expr);

	// Throw error for "if" & "elif" if not boolean.
	bool expr_passed = false;
	if (symbol != "else") {
		if (value.t != BOOL) {
			emit_error(ERR_expected_boolean_expression);
			return;
		}
		else {expr_passed = std::any_cast<bool>(value.d);}
	}

	bool previous_conditional_passed = true;
	if (token.linked_inst == "if" || token.linked_inst == "elif") {
		InstToken& linked_token = InstTokenSeq.at(token.i + token.linked_inst_pos);
		if (linked_token.meta.size() == 0) {
			emit_error(ERR_unexpected_else);
			return;
		}
		previous_conditional_passed = std::any_cast<bool>(linked_token.meta.at(0));
	}

	bool passed = false;
	if (symbol == "if") {passed = expr_passed;}
	else if (symbol == "elif") {passed = expr_passed && not previous_conditional_passed;}
	else if (symbol == "else") {passed = not previous_conditional_passed;}

	token.meta = {passed};
	if (symbol == "elif" && not passed) {token.meta.at(0) = previous_conditional_passed;}
	InstTokenSeq[token.i] = token;
	// Jump past instructions in this composite if failed.
	if (not passed) {exec_jump_value += token.composite_size;}
}


const Instruction INST_If {
	0,             // Required arg count.
	-1,            // Optional arg count.
	INST_If_exec,  // Function.
	true,          // Is composite.
};
