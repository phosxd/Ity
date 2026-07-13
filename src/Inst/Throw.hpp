#pragma once


void INST_Throw_exec(const Instruction* _inst, InstToken& _token, ScopeState& state, const std::vector<std::string>& args) {
	const unsigned int args_len = args.size();
	std::string expr;
	expr.reserve(args.size());
	for (unsigned int i = 1; i < args_len; i++) {
		expr += ' '+args[i];
	}

	if (expr.empty()) {
		emit_error(ERR_custom, {"Exception thrown."});
		return;
	}

	const Variant& var = expr_run(state, expr);
	if (var.t != STR) {
		emit_error(ERR_expected_string_expression);
		return;
	}

	emit_error(ERR_custom, {std::any_cast<STR_t>(var.d)});
}


const Instruction INST_Throw {
	0,                // Required arg count.
	-1,               // Optional arg count.
	INST_Throw_exec,  // Function.
	false,            // Is composite.
};
