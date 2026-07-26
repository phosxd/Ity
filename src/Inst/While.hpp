#pragma once


void INST_While_processor(const Instruction* inst, InstToken& token, const AnyMap_t& extra, const unsigned int& ln, const unsigned int& col) {
	const std::string& symbol = token.args[0];
	token.meta = {"", std::monostate(), (unsigned int)0};

	if (symbol == "for") {
		// Throw error if not enough args, or third arg is not the required keyword.
		if (token.args.size() < 4 || token.args[3] != "in") {
			emit_error(ERR_invalid_syntax, {"Expected keyword \"in\""}, ln, col);
			return;
		}

		const std::string& name = token.args[1];
		// Throw error if invalid name.
		if (not is_valid_name(name)) {
			emit_error(ERR_name_must_not_contain_symbols, {name}, ln, col);
			return;
		}

		token.meta[0] = name;
		token.args = tokenize_expr_from_inst_args(token, 3);
	}


	else if (symbol == "while") {
		token.args = tokenize_expr_from_inst_args(token, inst->REQUIRED);
	}
}




void INST_While_exec(ScopeState& state, const Instruction* _inst, InstToken& token) {
	const std::string& symbol = token.args[0];
	Variant value = VariantPresets.bool_false;

	// While loop.
	if (symbol == "while") {
		// Get value from expression.
		value = expr_exec(state, token.expr);
		// Throw error if not boolean.
		if (value.t != BOOL) {
			emit_error(ERR_expected_boolean_expression);
			return;
		}
	}


	// Jump past instructions in this composite if failed.
	if (not AnyCast(bool,value.d)) {
		exec_jump_value += token.composite_size; // Add 1 to skip the end instruction, otherwise will jump back to this instruction.
		// Scope out if previously scoped in.
		if (token.declarative_composite && token.meta.size() > 3) {
			scope_out(state);
			token.meta.pop_back();
			scoped_tokens.pop_back();
		}
		return;
	}
	// If entering loop for first time & the composite is declarative, then scope in.
	else if (token.declarative_composite && token.meta.size() == 3) {
		token.meta.push_back(true);
		scope_in(state);
		scoped_tokens.push_back(&token);
	}


	// For loop.
	// if (symbol == "for") {
	// 	const std::string& name = AnyCast(STR_t,token.meta[0]);
	// 	unsigned int index = AnyCast(unsigned int,token.meta[1]);
	// 	// Give warning if the var name is shadowing another var name.
	// 	if (not is_name_globally_free(state, name)) {
	// 		emit_warn(ERR_name_is_shadowed, {name});
	// 	}
 //
	// 	// Get iterable.
	// 	Variant iterable = expr_exec(state, token.expr);
	// 	Variant* item = nullptr;
	// 	Variant temp = VariantPresets.empty;
	// 	if (iterable.t == ARR) {
	// 		item = &AnyCastV(ARR_t,iterable.d)[index];
	// 	}
	// 	else if (iterable.t == STR) {
	// 		temp = Variant{STR, AnyCast(STR_t,iterable.d)[index]};
	// 		item = &temp;
	// 	}
	// 	else {
	// 		emit_error(ERR_invalid_syntax, {"Expected iterable expression"});
	// 		return;
	// 	}
 //
	// 	// Set item.
	// 	set_data(state, name, ANY, *item, VariantMode_dynamic_type);
	// }
}


const Instruction INST_While {
	1,                // Required arg count.
	INST_While_exec,  // Function.
	true,             // Is composite.
	false,            // Has expression.
	INST_While_processor,
};
