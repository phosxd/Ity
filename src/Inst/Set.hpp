#pragma once


void INST_Set_exec(const Instruction* inst, InstToken& _token, ScopeState& state, const std::vector<std::string>& args) {
	const unsigned int args_len = args.size();
	const std::string& symbol = args[0];
	const std::string& name = args[1];
	const std::string& op = args[2];
	std::string expr = "";
	expr.reserve(args_len-inst->REQUIRED);
	for (unsigned int i = 3; i < args_len; i++) {
		expr += ' '+args[i];
	}

	if (is_valid_name(name) == false) {
		emit_error(ERR_name_must_not_contain_symbols);
		return;
	}

	// Give error if the var name is not available on the current scope.
	if (is_name_free(state, name) == true) {
		emit_error(ERR_name_does_not_exist, {name});
		return;
	}

	// Get current var.
	const Variant& var = get_data_globally(state, name);
	// Get value from expression.
	current_column += count_non_empty_strings({symbol,name,op}) + symbol.size() + name.size() + op.size();
	const Variant& value = expr_run(state, expr);

	// Set variable data.
	if (op == "=" || op == "") {
		set_data(state, name, var.t, value.d, var.m);
	}
	else if (op == "+=") {
		set_data(state, name, var.t, (var.d+value.d), var.m);
	}
	else if (op == "-=") {
		set_data(state, name, var.t, (var.d-value.d), var.m);
	}
	else if (op == "*=") {
		set_data(state, name, var.t, (var.d*value.d), var.m);
	}
	else if (op == "/=") {
		set_data(state, name, var.t, (var.d/value.d), var.m);
	}
	// Throw error if invalid operator.
	else {
		emit_error(ERR_invalid_assignment_op, {symbol, op});
		return;
	}
}


const Instruction INST_Set {
	2,              // Required arg count.
	-1,             // Optional arg count.
	INST_Set_exec,  // Function.
	false,          // Is composite.
};
