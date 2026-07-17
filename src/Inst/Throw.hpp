#pragma once


void INST_Throw_exec(ScopeState& state, const Instruction* _inst, InstToken& token, const std::vector<std::string>& args) {
	if (token.expr.seq.empty()) {
		emit_error(ERR_custom, {"Exception thrown."});
		return;
	}

	const Variant& var = expr_exec(state, token.expr);
	if (var.t != STR) {
		emit_error(ERR_expected_string_expression);
		return;
	}

	emit_error(ERR_custom, {std::any_cast<STR_t>(var.d)});
}


const Instruction INST_Throw {
	1,                // Required arg count.
	INST_Throw_exec,  // Function.
	false,            // Is composite.
	true,             // Has expression.
};
