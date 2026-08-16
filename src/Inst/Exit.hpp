#pragma once


static void INST_Exit_exec(ItyState& state, InstToken& token) {
	// Exit with status code.
	if (token.symbol == InstSymbol_exit) {
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
	else if (token.symbol == InstSymbol_throw) {
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




const Instruction* INST_Exit = new Instruction{
	.REQUIRED = 1,
	.exec = INST_Exit_exec,
	.has_expr = true,
};
