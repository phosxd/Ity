#pragma once


void INST_Exit_exec(ScopeState& state, InstToken& token) {
	// Exit with status code.
	if (token.args[0] == "exit") {
		if (token.expr.seq.empty()) {
			exit(0);
			return;
		}

		const Variant* var = expr_exec(state, token.expr);
		if (var->t != INT) {
			emit_error(ERR_invalid_syntax, {"Expected integer expression"});
			return;
		}

		exit(AnyCast(INT_t,var->d));
	}

	// Throw error.
	else if (token.args[0] == "throw") {
		if (token.expr.seq.empty()) {
			emit_error(ERR_custom, {"Exception thrown."});
			return;
		}

		const Variant* var = expr_exec(state, token.expr);
		if (var->t != STR) {
			emit_error(ERR_invalid_syntax, {"Expected string expression"});
			return;
		}

		emit_error(ERR_custom, {AnyCast(STR_t,var->d)});
	}
}


const auto* INST_Exit = new Instruction{
	1,                // Required arg count.
	INST_Exit_exec,   // Function.
	false,            // Is composite.
	true,             // Has expression.
};
