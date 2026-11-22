#pragma once
// Fragment Size: 2178


struct INST_Loop_data {
	bool multi = false;
	// For-loop properties.
	STR_t vname = "";
	var_key_t hvname = 0;
	Variant iterable = VPS.empty;
	Variant item = VPS.empty;
	uint64_t index = 0;
};


static void INST_Loop_processor(InstToken& token, const InstProcArgs& args) {
	token.data = new INST_Loop_data{};
	auto& data = *(INST_Loop_data*)token.data;


	if (token.symbol == InstSymbol_for) {
		// Throw error if not enough args, or third arg is not the required keyword.
		if (token.args.size() < 4 || token.args[2] != "in") {
			emit_error(ERR_invalid_syntax, {"Expected keyword \"in\""}, args.ln, args.col);
			return;
		}

		const STR_t& name = token.args[1];
		// Throw error if invalid name.
		if (not is_valid_name(name)) {
			emit_error(ERR_name_must_not_contain_symbols, {name}, args.ln, args.col);
			return;
		}

		data.hvname = string_hasher(name);
		data.vname = std::move(name);
		token.args = tokenize_expr_from_inst_args(token, 3);
	}


	else if (token.symbol == InstSymbol_while) token.args = tokenize_expr_from_inst_args(token, token.inst->REQUIRED);
}




static void INST_Loop_for_loop(ItyState& state, InstToken& token, bool& value) {
	auto& data = *(INST_Loop_data*)token.data;

	const uint64_t& index = data.index;
	// Get iterable.
	Variant* iterable = &(data.iterable);
	if (iterable->t == VT_PLACEHOLDER) {
		data.iterable = *expr_exec(state, token.expr);
		iterable = &(data.iterable);
	}

	// Get item from number.
	if (iterable->is_number()) {
		if (iterable->to_uint64() > index) {
			value = true;
			data.item.t = VT_UINT64;
			data.item.d = index;
		}
	}
	else {
		switch (iterable->t) {
			// Get item from string.
			case VT_STR: {
				const STR_t& iter_data = AnyCast(STR_t,iterable->d);
				if (iter_data.size() > index) {
					value = true;
					data.item.t = VT_STR;
					data.item.d = (STR_t)(std::string(1,iter_data[index]));
				} break;
			}
			// Get item from array.
			case VT_ARR: {
				ARR_t& iter_data = AnyCastV(ARR_t,iterable->d);
				if (iter_data.size() > index) {
					value = true;
					data.item = iter_data[index];
				} break;
			}
			// Get item from function.
			case VT_FUNC: {
				ARR_t args = {data.item};
				const Variant& result = call_function(state, AnyCast(FUNC_t,iterable->d), args);
				if (result.t == VT_ARR) {
					if (const ARR_t& arr = AnyCast(ARR_t,result.d); arr.size() == 2 && arr[0].t == VT_BOOL) {
						value = AnyCast(bool,arr[0].d);
						data.item = arr[1];
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
}




static void INST_Loop_exec(ItyState& state, InstToken& token) {
	auto& data = *(INST_Loop_data*)token.data;
	bool value = false;


	// While loop.
	if (token.symbol == InstSymbol_while) {
		// Get value from expression.
		const Variant* var = expr_exec(state, token.expr);
		// Throw error if not boolean.
		if (var->t != VT_BOOL) {
			emit_error(ERR_expected_boolean_expression);
			return;
		}
		value = AnyCast(bool,var->d);
	}


	// For loop.
	else if (token.symbol == InstSymbol_for) {
		INST_Loop_for_loop(state, token, value);
	}


	const bool multi = (token.declarative_composite || token.symbol == InstSymbol_for);

	// Jump past instructions in this composite if failed.
	if (not value) {
		state.exec_jump_value += token.composite_size; // Add 1 to skip the end instruction, otherwise will jump back to this instruction.
		// Scope out if previously scoped in.
		if (multi && data.multi) {
			state.scope.out();
			state.scoped_tokens.pop_back();
			data.multi = false;
		}

		// Reset token state.
		data.iterable = VPS.empty;
		data.item = VPS.empty;
		data.index = 0;
		return;
	}

	// If entering loop for first time & (the composite is declarative or is a for loop), then scope in.
	else if (multi && not data.multi) {
		data.multi = true;
		state.scope.in(state.scope_current_id);
		state.scoped_tokens.push_back(&token);

		if (token.symbol == InstSymbol_for) {
			// Give warning if the var name is shadowing another var name.
			const STR_t& var_name = data.vname;
			if (state.scope.p->get_data_globally(var_name, nullptr, data.hvname)) {
				emit_warn(ERR_name_is_shadowed, {var_name});
			}
		}
	}


	// If for loop, set the variable.
	if (token.symbol == InstSymbol_for && value) {
		state.scope.set_data(data.vname, Variant{data.item.t, std::move(data.item.d), VM_dynamic_type}, data.hvname);
		data.index += 1;
	}
}



static void INST_Loop_emergency_scope_exit(InstToken*& token) {
	auto& data = *(INST_Loop_data*)token->data;

	// Reset token state.
	data.multi = false;
	data.iterable = VPS.empty;
	data.item = VPS.empty;
	data.index = 0;
}




const Instruction* INST_Loop = new Instruction{
	.REQUIRED = 1,
	.exec = INST_Loop_exec,
	.is_composite = true,
	.processor = INST_Loop_processor,
	.emergency_scope_exit = INST_Loop_emergency_scope_exit
};
