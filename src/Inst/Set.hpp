#pragma once

#include "../Common.hpp"
#include "../ScriptErrors.hpp"


void INST_Set_exec(Instruction& inst, ScopeState& state, std::vector<std::string> args, std::string symbol) {
	const unsigned int args_len = args.size();
	std::string name;
	std::string op = "";
	std::string expr = "";
	for (unsigned int i = 0; i < args_len; i++) {
		if (i == 0) {continue;}
		if (i == 1) {name = args[i];}
		else if (i == 2) {op = args[i];}
		else {
			expr += args[i];
		}
	}
	current_column += 3 + symbol.size() + name.size() + op.size();

	if (is_valid_name(name) == false) {
		emit_error("Name must not contain any symbols.");
		return;
	}

	// Give error if the var name is not available on the current scope.
	if (is_name_free(state, name) == true) {
		emit_error(err_name_does_not_exist(name));
		return;
	}

	// Get current var.
	Variant var = get_data_globally(state, name);
	// Get value from expression.
	Variant value = expr_exec(state, expr);

	// Set variable data.
	if (op == "=" || op == "") {
		set_data(state, name, var.t, value.d, var.m);
	}
	else if (op == "+=") {
		VariantData sum = (var.d + value.d);
		set_data(state, name, var.t, sum, var.m);
	}
	// Throw error if invalid operator.
	else {
		emit_error(err_invalid_assignment_op(args[0], op));
		return;
	}
}


Instruction INST_Set {
	2, // "REQUIRED". Name & Operator.
	-1, // "OPTIONAL". Other parts of Expression.
	INST_Set_exec,
};
