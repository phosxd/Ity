#pragma once

#include "../Common.hpp"
#include "../ScriptErrors.hpp"


void INST_Var_exec(const Instruction& inst, ScopeState& state, const std::vector<std::string>& args, const std::string& symbol) {
	const unsigned int args_len = args.size();
	std::string type_name;
	std::string name;
	std::string op = "";
	std::string expr = "";
	expr.reserve(args_len-inst.REQUIRED);
	for (unsigned int i = 0; i < args_len; i++) {
		if (i == 0) {continue;}
		if (i == 1) {type_name = args[i];}
		else if (i == 2) {name = args[i];}
		else if (i == 3) {op = args[i];}
		else {
			expr += ' '+args[i];
		}
	}

	if (is_valid_name(name) == false) {
		emit_error("Name must not contain any symbols.");
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

	// Set variable mode.
	VariantType type = get_variant_type_from_name(type_name);
	unsigned int mode = 2; // Locked type.
	if (args[0] == "const") {mode = 1;} // Constant.
	if (type == ANY) {
		if (mode == 1) {
			emit_error("Constant must have an explicit type, not \"ANY\".");
			return;
		}
		mode = 0; // Dynamic type.
	}

	// Get value from expression.
	current_column += count_non_empty_strings({symbol,type_name,name,op}) + symbol.size() + type_name.size() + name.size() + op.size();
	Variant value = expr_run(state, expr);

	// Infer the variable's type as expression return type.
	if (type == INFERRED) {
		type = value.t;
	}

	// Set variable data.
	if (op == "=" || op == "") {
		set_data(state, name, type, value.d, mode);
	}
	// Throw error if invalid operator.
	else {
		emit_error(err_invalid_assignment_op(args[0], op));
		return;
	}
}


Instruction INST_Var {
	3, // "REQUIRED". Type, Name, & Operator.
	-1, // "OPTIONAL". Other parts of Expression.
	INST_Var_exec,
};
