#pragma once


void INST_Var_processor(InstToken& token, const AnyMap_t& _extra, const unsigned int& ln, const unsigned int& col) {
	std::string type_name = "*";
	std::string name;
	std::string op = "";
	std::string expr;
	std::string buffer;
	bool is_expr = false;
	unsigned int ln_ = 0;
	unsigned int col_ = 0;
	unsigned int arg_count = 0;
	// Parse arguments for type name, var name, operator, & expression.
	for (const char& ch : join_str(std::vector<std::string>(token.args.begin()+1, token.args.end()), " ")+' ') {
		if (is_expr) {
			expr += ch;
			continue;
		}
		LN_COL_COUNTER(ch,ln_,col_);

		// If encountered the only valid operator...
		if (ch == '=') {
			if (not buffer.empty()) {
				name = buffer;
			}
			// If we only found 1 argument before the operator then set `name` to `type_name`, & type name should be inferred instead.
			else if (arg_count == 1) {
				name = type_name;
				type_name = "*";
			}
			// Set `op`.
			op = ch;
			is_expr = true; // Expect everything after to be an expression.
			continue;
		}

		// If new argument...
		if (ch == ' ') {
			arg_count += 1;
			if (arg_count == 1) type_name = buffer;
			else if (arg_count == 2) name = buffer;
			buffer.clear();
			continue;
		}
		buffer += ch;
	}

	if (arg_count == 1 && not is_expr) {
		name = type_name;
		type_name = "*";
	}


	// Throw error if invalid name.
	if (not is_valid_name(name)) {
		emit_error(ERR_name_must_not_contain_symbols, {name}, ln, col);
		return;
	}

	VariantType type = get_variant_type_from_name(type_name);
	VariantMode mode = VariantMode_locked_type;
	if (token.symbol == InstSymbol_const) mode = VariantMode_constant;
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
	const std::string& name = AnyCast(std::string,token.meta[0]);
	//const std::string& op = AnyCast(std::string,token.meta[1]);

	// Give error if the var name is not free on the current scope.
	if (not is_name_free(state, name)) {
		emit_error(ERR_name_is_taken, {name});
		return;
	}

	// Give warning if the var name is shadowing another var name.
	if (not is_name_globally_free(state, name)) {
		emit_warn(ERR_name_is_shadowed, {name});
	}

	// Get value from expression.
	Variant var = *expr_exec(state, token.expr);


	if (token.symbol == InstSymbol_arg) {
		// Throw error if this scope holds no arguments.
		if (is_name_free(state, "__AG")) {
			emit_error(ERR_no_args_available);
			return;
		}
		// Get argument if available.
		ARR_t& scope_args = AnyCastV(ARR_t,get_data(state, "__AG")->d);
		if (not scope_args.empty()) {
			var = scope_args.front();
			scope_args.erase(scope_args.begin());
		}
	}

	// Get variable type & infer it if needed.
	VariantType type = AnyCastV(VariantType,token.meta[2]);
	if (type == INFERRED) type = var.t;
	// Set value to sane default if not explicitly set.
	else if (var.t == NONE && var.t != type) {
		switch (type) {
			case REF:    {var = Variant{REF, (STR_t)"noneref"}; break;}
			case BOOL:   {var = VariantPresets.bool_false;      break;}
			case INT:    {var = Variant{INT, (INT_t)0};         break;}
			case FLOAT:  {var = Variant{FLOAT, (FLOAT_t)0};     break;}
			case STR:    {var = Variant{STR, (STR_t)""};        break;}
			case ARR:    {var = Variant{ARR, (ARR_t){}};        break;}
			case MAP:    {var = Variant{MAP, (MAP_t){}};        break;}
			default: break;
		}
	}
	// Set data.
	set_data(state, name, type, var, AnyCast(VariantMode,token.meta[3]));
}




const Instruction* INST_Var = new Instruction{
	2,              // Required arg count.
	INST_Var_exec,  // Function.
	false,          // Is composite.
	false,          // Has expression. (Manually handled for this Instruction).
	INST_Var_processor,
};
