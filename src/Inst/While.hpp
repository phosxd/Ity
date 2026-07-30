#pragma once


void INST_While_processor(InstToken& token, const AnyMap_t& extra, const unsigned int& ln, const unsigned int& col) {
	const std::string& symbol = token.args[0];
	token.meta = {
		false,     // Multi.

		(STR_t)"", // For loop item var name.
		(ARR_t){
			VariantPresets.empty, // For loop iterable.
			VariantPresets.empty, // For loop current item in iterable.
		},
		(unsigned int)0,  // For loop iteration index.
	};


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

		token.meta[1] = std::move(name);
		token.args = tokenize_expr_from_inst_args(token, 3);
	}


	else if (symbol == "while") {
		token.args = tokenize_expr_from_inst_args(token, token.inst->REQUIRED);
	}
}




void INST_While_for_loop(ScopeState& state, InstToken& token, bool& value) {
	const unsigned int& index = AnyCast(unsigned int,token.meta[3]);
	// Get iterable.
	Variant& iterable = AnyCastV(ARR_t,token.meta[2])[0];
	if (iterable.t == PLACEHOLDER) {
		iterable = Variant(*expr_exec(state, token.expr));
		AnyCastV(ARR_t,token.meta[2])[0] = iterable;
	}

	// Get item from integer.
	if (iterable.t == INT) {
		if (AnyCast(INT_t,iterable.d) > (INT_t)index) {
			value = true;
			AnyCastV(ARR_t,token.meta[2])[1] = Variant{INT, (INT_t)index};
		}
	}

	// Get item from string.
	else if (iterable.t == STR) {
		const STR_t& data = AnyCast(STR_t,iterable.d);
		if (data.size() > index) {
			value = true;
			AnyCastV(ARR_t,token.meta[2])[1] = Variant{STR, (STR_t)(std::string(1,data[index]))};
		}
	}

	// Get item from array.
	else if (iterable.t == ARR) {
		ARR_t& data = AnyCastV(ARR_t,iterable.d);
		if (data.size() > index) {
			value = true;
			AnyCastV(ARR_t,token.meta[2])[1] = data[index];
		}
	}

	// Get item from iterable object.
	else if (iterable.t == MAP) {
		const MAP_t& map = AnyCast(MAP_t,iterable.d);
		if (const auto& it = map.find("__iter"); it != map.end()) {
			value = true;
			Variant args {
				ARR,(ARR_t){
					Variant{PTR, &iterable},
					Variant{INT, (INT_t)index},
				}
			};
			const Variant& result = call_script_function(state, AnyCast(MAP_t,it->second.d), args);
			if (result.t == ARR) {
				if (const ARR_t& arr = AnyCast(ARR_t,result.d); arr.size() == 2 && arr[0].t == BOOL) {
					value = AnyCast(bool,arr[0].d);
					AnyCastV(ARR_t,token.meta[2])[1] = arr[1];
				}
			}
		}
	}

	// Throw error if variant is not iterable.
	else {
		emit_error(ERR_invalid_syntax, {"Expected iterable expression"});
		return;
	}
}




void INST_While_exec(ScopeState& state, InstToken& token) {
	const std::string& symbol = token.args[0];
	bool value = false;


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
		INST_While_for_loop(state, token, value);
	}


	const bool multi = (token.declarative_composite || symbol == "for");

	// Jump past instructions in this composite if failed.
	if (not value) {
		exec_jump_value += token.composite_size; // Add 1 to skip the end instruction, otherwise will jump back to this instruction.
		// Scope out if previously scoped in.
		if (multi && AnyCast(bool,token.meta[0])) {
			scope_out(state);
			scoped_tokens.pop_back();
			token.meta[0] = false;
		}

		// Reset token state.
		token.meta[2] = (ARR_t){VariantPresets.empty, VariantPresets.empty};
		token.meta[3] = (unsigned int)0;
		return;
	}

	// If entering loop for first time & (the composite is declarative or is a for loop), then scope in.
	else if (multi && not AnyCast(bool,token.meta[0])) {
		token.meta[0] = true;
		scope_in(state);
		scoped_tokens.push_back(&token);

		if (symbol == "for") {
			// Give warning if the var name is shadowing another var name.
			if (not is_name_globally_free(*(state.p), AnyCast(STR_t,token.meta[1]))) {
				emit_warn(ERR_name_is_shadowed, {AnyCast(STR_t,token.meta[1])});
			}
		}
	}


	// If for loop, set the variable.
	if (symbol == "for" && value) {
		set_data(state, AnyCast(STR_t,token.meta[1]), AnyCastV(ARR_t,token.meta[2])[1].t, std::move(AnyCastV(ARR_t,token.meta[2])[1]), VariantMode_dynamic_type);
		AnyCastV(ARR_t,token.meta[2])[1] = VariantPresets.empty;
		token.meta[3] = AnyCast(unsigned int,token.meta[3]) + 1;
	}
}



void INST_While_emergency_scope_exit(InstToken*& token) {
	// Reset token state.
	token->meta[0] = false;
	token->meta[2] = (ARR_t){VariantPresets.empty, VariantPresets.empty};
	token->meta[3] = (unsigned int)0;
}




const Instruction INST_While {
	1,                // Required arg count.
	INST_While_exec,  // Function.
	true,             // Is composite.
	false,            // Has expression.
	INST_While_processor,
	INST_While_emergency_scope_exit,
};
