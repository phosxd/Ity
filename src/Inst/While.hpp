#pragma once


void INST_While_processor(InstToken& token, const AnyMap_t& extra, const unsigned int& ln, const unsigned int& col) {
	const std::string& symbol = token.args[0];
	token.meta = {(STR_t)"", std::monostate(), (unsigned int)0};

	if (symbol == "for") {
		// Throw error if not enough args, or third arg is not the required keyword.
		if (token.args.size() < 4 || token.args[2] != "in") {
			emit_error(ERR_invalid_syntax, {"Expected keyword \"in\""}, ln, col);
			return;
		}

		const STR_t& name = token.args[1];
		// Throw error if invalid name.
		if (not is_valid_name(name)) {
			emit_error(ERR_name_must_not_contain_symbols, {name}, ln, col);
			return;
		}

		token.meta[0] = std::move(name);
		token.args = tokenize_expr_from_inst_args(token, 3);
	}


	else if (symbol == "while") {
		token.args = tokenize_expr_from_inst_args(token, token.inst->REQUIRED);
	}
}




void INST_While_exec(ScopeState& state, InstToken& token) {
	const std::string& symbol = token.args[0];
	bool value = false;

	// For loop variables.
	Variant item;
	const std::string name = AnyCast(STR_t,token.meta[0]);


	// While loop.
	if (symbol == "while") {
		// Get value from expression.
		const Variant* var = expr_exec(state, token.expr);
		// Throw error if not boolean.
		if (var->t != BOOL) {
			emit_error(ERR_expected_boolean_expression);
			return;
		}
		value = AnyCast(bool,var->d);
	}


	// For loop.
	else if (symbol == "for") {
		const unsigned int& index = AnyCast(unsigned int,token.meta[2]);
		// Get iterable.
		Variant* iterable = nullptr;
		if (const auto& i = std::get_if<Variant*>(&token.meta[1])) iterable = *i;
		else {
			iterable = new Variant(*expr_exec(state, token.expr));
			token.meta[1] = iterable;
		}

		// Get item from array.
		if (iterable->t == ARR) {
			ARR_t& data = AnyCastV(ARR_t,iterable->d);
			if (data.size() <= index) value = false;
			else {
				value = true;
				item = data[index];
			}
		}

		// Get item from string.
		else if (iterable->t == STR) {
			const STR_t& data = AnyCast(STR_t,iterable->d);
			if (data.size() <= index) value = false;
			else {
				value = true;
				item = Variant{STR, (STR_t)(std::string(1,data[index]))};
			}
		}

		// Get item from integer.
		else if (iterable->t == INT) {
			const INT_t& data = AnyCast(INT_t,iterable->d);
			if (data <= (INT_t)index) value = false;
			else {
				value = true;
				item = Variant{INT, (INT_t)index};
			}
		}

		// Throw error if variant is not iterable.
		else {
			emit_error(ERR_invalid_syntax, {"Expected iterable expression"});
			return;
		}
	}


	// Jump past instructions in this composite if failed.
	if (not value) {
		exec_jump_value += token.composite_size; // Add 1 to skip the end instruction, otherwise will jump back to this instruction.
		// Scope out if previously scoped in.
		if ((token.declarative_composite || symbol == "for") && token.meta.size() > 3) {
			scope_out(state);
			scoped_tokens.pop_back();
			token.meta.pop_back();
		}

		// Reset token state.
		token.meta[1] = std::monostate();
		token.meta[2] = (unsigned int)0;
		return;
	}

	// If entering loop for first time & (the composite is declarative or is a for loop), then scope in.
	else if ((token.declarative_composite || symbol == "for") && token.meta.size() == 3) {
		token.meta.push_back(true);
		scope_in(state);
		scoped_tokens.push_back(&token);

		if (symbol == "for") {
			// Give warning if the var name is shadowing another var name.
			if (not is_name_globally_free(*(state.p), name)) {
				emit_warn(ERR_name_is_shadowed, {name});
			}
		}
	}


	// If for loop, set the variable.
	if (symbol == "for" && value) {
		set_data(state, name, item.t, std::move(item), VariantMode_dynamic_type);
		token.meta[2] = AnyCast(unsigned int,token.meta[2]) + 1;
	}
}



void INST_While_emergency_scope_exit(InstToken*& token) {
	// Reset token state.
	token->meta[1] = std::monostate();
	token->meta[2] = (unsigned int)0;
	token->meta.pop_back();
}




const Instruction INST_While {
	1,                // Required arg count.
	INST_While_exec,  // Function.
	true,             // Is composite.
	false,            // Has expression.
	INST_While_processor,
	INST_While_emergency_scope_exit,
};
