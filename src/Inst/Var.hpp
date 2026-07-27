#pragma once


void INST_Var_processor(InstToken& token, const AnyMap_t& extra, const unsigned int& ln, const unsigned int& col) {
	const std::string& type_name = token.args[1];
	std::string name;
	std::string op = "";
	std::string expr;
	bool is_expr = false;
	unsigned int ln_ = 0;
	unsigned int col_ = 0;
	for (const char& ch : join_str(std::vector<std::string>(token.args.begin()+2, token.args.end()), " ")) {
		if (is_expr) {
			expr += ch;
			continue;
		}
		LN_COL_COUNTER(ch,ln_,col_);
		if (ch == '=') {
			op = ch;
			is_expr = true;
			continue;
		}
		if (ch == ' ') continue;
		name += ch;
	}

	// Throw error if invalid name.
	if (not is_valid_name(name)) {
		emit_error(ERR_name_must_not_contain_symbols, {name}, ln, col);
		return;
	}

	VariantType type = get_variant_type_from_name(type_name);
	VariantMode mode = VariantMode_locked_type;
	if (token.args[0] == "const") mode = VariantMode_constant;
	if (type == ANY) {
		if (mode == 1) {
			emit_error(ERR_constant_type_not_explicit);
			return;
		}
		mode = VariantMode_dynamic_type;
	}

	// Set token properties.
	token.meta = {name, op, type, mode};
	token.expr = expr_tokenize(expr, ln-ln_, col-col_);
}




void INST_Var_exec(ScopeState& state, InstToken& token) {
	const std::string& symbol = token.args[0];
	const std::string& name = AnyCast(std::string,token.meta[0]);
	const std::string& op = AnyCast(std::string,token.meta[1]);

	// Give error if the var name is not free on the current scope.
	if (not is_name_free(state, name)) {
		emit_error(ERR_name_is_taken, {name});
		return;
	}

	// Give warning if the var name is shadowing another var name.
	if (not is_name_globally_free(state, name)) {
		emit_warn(ERR_name_is_shadowed, {name});
	}

	// Get variable type & mode.
	VariantType type = AnyCastV(VariantType,token.meta[2]);
	const VariantMode& mode = AnyCast(VariantMode,token.meta[3]);
	// Get value from expression.
	Variant value = expr_exec(state, token.expr);
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
}


const Instruction INST_Var {
	3,              // Required arg count.
	INST_Var_exec,  // Function.
	false,          // Is composite.
	false,          // Has expression. (Manually handled for this Instruction).
	INST_Var_processor,
};
