#pragma once


static void INST_Loop_processor(InstToken& token, const AnyMap_t& _extra, const unsigned int& ln, const unsigned int& col) {
	token.meta = {
		false,     // Multi.

		(STR_t)"", // For loop item var name.
		(size_t)0, // For loop item hashed var name.
		(ARR_t){
			VariantPresets.empty, // For loop iterable.
			VariantPresets.empty, // For loop current item in iterable.
		},
		(unsigned int)0,  // For loop iteration index.
	};


	if (token.symbol == InstSymbol_for) {
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

		token.meta[2] = string_hasher(name);
		token.meta[1] = std::move(name);
		token.args = tokenize_expr_from_inst_args(token, 3);
	}


	else if (token.symbol == InstSymbol_while) {
		token.args = tokenize_expr_from_inst_args(token, token.inst->REQUIRED);
	}
}




static void INST_Loop_for_loop(ItyState& state, InstToken& token, bool& value) {
	const unsigned int& index = AnyCast(unsigned int,token.meta[4]);
	// Get iterable.
	Variant& iterable = AnyCastV(ARR_t,token.meta[3])[0];
	if (iterable.t == PLACEHOLDER) {
		iterable = *expr_exec(state, token.expr);
		AnyCastV(ARR_t,token.meta[3])[0] = iterable;
	}

	switch (iterable.t) {
		// Get item from integer.
		case INT: {
			if (AnyCast(INT_t,iterable.d) > (INT_t)index) {
				value = true;
				AnyCastV(ARR_t,token.meta[3])[1] = Variant{INT, (INT_t)index};
			} break;
		}
		// Get item from string.
		case STR: {
			const STR_t& data = AnyCast(STR_t,iterable.d);
			if (data.size() > index) {
				value = true;
				AnyCastV(ARR_t,token.meta[3])[1] = Variant{STR, (STR_t)(std::string(1,data[index]))};
			} break;
		}
		// Get item from array.
		case ARR: {
			ARR_t& data = AnyCastV(ARR_t,iterable.d);
			if (data.size() > index) {
				value = true;
				AnyCastV(ARR_t,token.meta[3])[1] = data[index];
			} break;
		}
		// Get item from iterable object.
		case MAP: {
			const MAP_t& map = AnyCast(MAP_t,iterable.d);
			if (const auto& it = map.find("__iter"); it != map.end()) {
				value = true;
				Variant args {
					ARR,(ARR_t){
						Variant{PTR, &iterable},
						Variant{INT, (INT_t)index},
					}
				};
				const Variant& result = call_function(state, AnyCast(FUNC_t,it->second.d), args);
				if (result.t == ARR) {
					if (const ARR_t& arr = AnyCast(ARR_t,result.d); arr.size() == 2 && arr[0].t == BOOL) {
						value = AnyCast(bool,arr[0].d);
						AnyCastV(ARR_t,token.meta[3])[1] = arr[1];
					}
				}
			} break;
		}
		// Throw error if variant is not iterable.
		default: {
			emit_error(ERR_not_iterable);
			return;
		}
	}
}




static void INST_Loop_exec(ItyState& state, InstToken& token) {
	bool value = false;


	// While loop.
	if (token.symbol == InstSymbol_while) {
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
	else if (token.symbol == InstSymbol_for) {
		INST_Loop_for_loop(state, token, value);
	}


	const STR_t& var_name = AnyCast(STR_t,token.meta[1]);
	const bool multi = (token.declarative_composite || token.symbol == InstSymbol_for);

	// Jump past instructions in this composite if failed.
	if (not value) {
		exec_jump_value += token.composite_size; // Add 1 to skip the end instruction, otherwise will jump back to this instruction.
		// Scope out if previously scoped in.
		if (multi && AnyCast(bool,token.meta[0])) {
			state.scope.out();
			scoped_tokens.pop_back();
			token.meta[0] = false;
		}

		// Reset token state.
		token.meta[3] = (ARR_t){VariantPresets.empty, VariantPresets.empty};
		token.meta[4] = (unsigned int)0;
		return;
	}

	// If entering loop for first time & (the composite is declarative or is a for loop), then scope in.
	else if (multi && not AnyCast(bool,token.meta[0])) {
		token.meta[0] = true;
		state.scope.in();
		scoped_tokens.push_back(&token);

		if (token.symbol == InstSymbol_for) {
			// Give warning if the var name is shadowing another var name.
			if (state.scope.p->get_data_globally(var_name, nullptr, AnyCast(size_t,token.meta[2]))) {
				emit_warn(ERR_name_is_shadowed, {var_name});
			}
		}
	}


	// If for loop, set the variable.
	if (token.symbol == InstSymbol_for && value) {
		Variant& var = AnyCastV(ARR_t,token.meta[3])[1];
		state.scope.set_data(var_name, var.t, std::move(var), VariantMode_dynamic_type, AnyCast(size_t,token.meta[2]));
		AnyCastV(ARR_t,token.meta[3])[1] = VariantPresets.empty;
		token.meta[4] = AnyCast(unsigned int,token.meta[4]) + 1;
	}
}



static void INST_Loop_emergency_scope_exit(InstToken*& token) {
	// Reset token state.
	token->meta[0] = false;
	token->meta[3] = (ARR_t){VariantPresets.empty, VariantPresets.empty};
	token->meta[4] = (unsigned int)0;
}




const Instruction* INST_Loop = new Instruction{
	.REQUIRED = 1,
	.exec = INST_Loop_exec,
	.is_composite = true,
	.processor = INST_Loop_processor,
	.emergency_scope_exit = INST_Loop_emergency_scope_exit
};
