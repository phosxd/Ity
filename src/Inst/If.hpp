#pragma once


bool previous_conditional_passed = true;
bool chain_start_passed = true;


void INST_If_exec(const Instruction& inst, const InstToken& token, ScopeState& state, const std::vector<std::string>& args) {
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
			emit_error("Expected a boolean result in expression.");
			return;
		}
		else {expr_passed = std::get<bool>(value.d);}
	}

	bool passed = false;
	if (symbol == "if") {passed = expr_passed; chain_start_passed = passed;}
	else if (symbol == "elif") {passed = expr_passed && not previous_conditional_passed;}
	else if (symbol == "else") {passed = not chain_start_passed && not previous_conditional_passed;}
	previous_conditional_passed = passed;
	// Jump past instructions in this composite if failed.
	if (not passed) {exec_jump_value += token.composite_size;}
}


const Instruction INST_If {
	0,             // Required arg count.
	-1,            // Optional arg count.
	INST_If_exec,  // Function.
	true,          // Is composite.
};
