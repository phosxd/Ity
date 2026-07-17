#pragma once


void INST_Set_exec(ScopeState& state, const Instruction* inst, InstToken& _token, const std::vector<std::string>& args) {
	const size_t& args_len = args.size();
	const std::string& symbol = args.at(0);
	const std::string& name = args.at(1);
	const std::string& op = args.at(2);
	std::string expr = ""; expr.reserve(args_len-inst->REQUIRED);
	for (size_t i = 3; i < args_len; i++) {
		expr += ' '+args[i];
	}

	if (not is_valid_name(name)) {
		emit_error(ERR_name_must_not_contain_symbols);
		return;
	}

	// Give error if the var name is not available on the current scope.
	if (is_name_globally_free(state, name)) {
		emit_error(ERR_name_does_not_exist, {name});
		return;
	}

	// Get current var.
	const Variant& var = get_data_globally(state, name);
	// Get value from expression.
	current_column += count_non_empty_strings({symbol,name,op}) + symbol.size() + name.size() + op.size();
	const Variant& value = expr_run(state, expr);

	// Set variable data.
	VariantData data;
	if (op == "=" || op == "") data = value.d;
	else if (op == "+=") data = var.d + value.d;
	else if (op == "-=") data = var.d - value.d;
	else if (op == "*=") data = var.d * value.d;
	else if (op == "/=") data = var.d / value.d;
	// Throw error if invalid operator.
	else {
		emit_error(ERR_invalid_assignment_op, {op, symbol});
		return;
	}

	set_data_globally(state, name, var.t, data, var.m);
}


const Instruction INST_Set {
	2,              // Required arg count.
	-1,             // Optional arg count.
	INST_Set_exec,  // Function.
	false,          // Is composite.
};
