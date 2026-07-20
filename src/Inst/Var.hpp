#pragma once


void INST_Var_exec(ScopeState& state, const Instruction* inst, InstToken& token, const std::vector<std::string>& args) {
	const size_t& args_len = args.size();
	const std::string& symbol = args[0];
	const std::string& type_name = args[1];
	const std::string& name = args[2];
	const std::string& op = (args_len>3) ? args[3] : "";
	std::string expr; expr.reserve(args_len-inst->REQUIRED);
	for (size_t i = 4; i < args_len; i++) {
		expr += ' '+args[i];
	}

	if (not is_valid_name(name)) {
		emit_error(ERR_name_must_not_contain_symbols, {name});
		return;
	}

	// Give error if the var name is not free on the current scope.
	if (not is_name_free(state, name)) {
		emit_error(ERR_name_is_taken, {name});
		return;
	}

	// Give warning if the var name is shadowing another var name.
	if (not is_name_globally_free(state, name)) {
		emit_warn(ERR_name_is_shadowed, {name});
	}

	// Set variable mode.
	VariantType type = get_variant_type_from_name(type_name);
	VariantMode mode = VariantMode_locked_type;
	if (symbol == "const") mode = VariantMode_constant;
	if (type == ANY) {
		if (mode == 1) {
			emit_error(ERR_constant_type_not_explicit);
			return;
		}
		mode = VariantMode_dynamic_type;
	}

	// Get value from expression.
	current_column += count_non_empty_strings({symbol,type_name,name,op}) + symbol.size() + type_name.size() + name.size() + op.size();
	const Variant& value = expr_run(state, expr);

	// Infer the variable's type as expression return type.
	if (type == INFERRED) type = value.t;

	// Set variable data.
	if (op == "=" || op == "") {
		if (symbol == "arg") {
			// Throw error if this scope holds no arguments.
			if (is_name_free(state, "__ARGS__")) {
				emit_error(ERR_no_args_available);
				return;
			}
			// Replace value if argument is available.
			const ARR_t& scope_args = AnyCast(ARR_t,get_data(state, "__ARGS__")->d);
			if (func_arg_index < scope_args.size()) {
				value = scope_args[func_arg_index];
				func_arg_index += 1;
			}
		}
		set_data(state, name, type, value.d, mode);
	}
	// Throw error if invalid operator.
	else {
		emit_error(ERR_invalid_assignment_op, {op, symbol});
		return;
	}
}


const Instruction INST_Var {
	3,              // Required arg count.
	INST_Var_exec,  // Function.
	false,          // Is composite.
	false           // Has expression. (Manually handled for this Instruction).
};
