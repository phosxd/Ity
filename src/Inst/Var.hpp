#pragma once

#include <string>
#include <iostream>

#include "../Common.hpp"
#include "../ScriptErrors.hpp"


void INST_Var_exec(Instruction& inst, ScopeState& state, std::vector<std::string> args) {
	const unsigned int args_len = args.size();
	std::string type;
	std::string name;
	std::string op;
	std::string expr = "";
	for(unsigned int i = 0; i < args_len; i++) {
		if (i == 0) {continue;}
		if (i == 1) {type = args[i];}
		else if (i == 2) {name = args[i];}
		else if (i == 3) {op = args[i];}
		else {
			expr += args[i];
		}
	}

	if (is_valid_name(name) == false) {
		emit_error("Name must not contain any numbers or symbols.");
		return;
	}

	// Give error if the var name is not free on the current scope.
	if (is_name_free(state, name) == false) {
		emit_error("Name \"" + name + "\" is already taken within this scope. Use another name for this variable.");
		return;
	}

	// Give warning if the var name is shadowing another var name.
	if (is_name_globally_free(state, name) == false) {
		emit_warn("Name \"" + name + "\" is shadowing another variable with the same name, you will not be able to access the shadowed variable unless you change the name.");
	}

	// Get value from expression.
	Variant value = expr_exec(state, expr);
	if (type != "ANY" && value.t != type) {
		emit_error("Cannot assign value of type \"" + value.t + "\" to variable of type \"" + type + "\".");
		return;
	}

	// Set variable mode.
	unsigned int mode = 2; // Locked type.
	if (args[0] == "const") {mode = 1;} // Constant.
	if (type == "ANY") {
		if (mode == 1) {
			emit_error("Constant must have an explicit type, not \"ANY\".");
			return;
		}
		mode = 0; // Dynamic type.
	}

	// Set.
	if (op == "=") {
		set_data(state, name, value.t, value.d, mode);
	}

	else {
		emit_error(err_invalid_assignment_op(args[0], op));
		return;
	}

	return;
}


Instruction INST_Var {
	4, // "REQUIRED". Type, Name, Operator, & first part of Expression.
	-1, // "OPTIONAL". Other parts of Expression.
	INST_Var_exec,
};
