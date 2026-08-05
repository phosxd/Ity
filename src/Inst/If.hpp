#pragma once



void INST_If_processor(InstToken& token, const AnyMap_t& extra, const unsigned int& ln, const unsigned int& col) {
	if (token.symbol == InstSymbol_elif || token.symbol == InstSymbol_else) {
		const CompositeItem& last_comp_item = *AnyCast(CompositeItem*,extra.at("lci"));

		// Throw error if previous composite item was not a valid conditional.
		if (last_comp_item.token.symbol != InstSymbol_if && last_comp_item.token.symbol != InstSymbol_elif) {
			emit_error(ERR_unexpected_inst, {token.args[0]}, ln,col);
			return;
		}

		// Link token to the previous conditional.
		token.linked_inst = last_comp_item.token.symbol;
		token.linked_inst_pos = -(int32_t)AnyCast(uint16_t,extra.at("lcid"));
	}
}




void INST_If_exec(ScopeState& state, InstToken& token) {
	// Get value from expression.
	const Variant* var = expr_exec(state, token.expr);

	// Throw error for "if" & "elif" if not boolean.
	bool expr_passed = false;
	if (token.symbol != InstSymbol_else) {
		if (var->t != BOOL) {
			emit_error(ERR_expected_boolean_expression);
			return;
		}
		else expr_passed = AnyCast(bool,var->d);
	}

	bool previous_conditional_passed = true;
	if (token.linked_inst == InstSymbol_if || token.linked_inst == InstSymbol_elif) {
		const InstToken& linked_token = InstTokenSeq[token.i + token.linked_inst_pos];
		if (linked_token.meta.size() == 0) {
			emit_error(ERR_unexpected_inst, {InstSymbol_to_string(token.symbol)});
			return;
		}
		previous_conditional_passed = AnyCast(bool,linked_token.meta[0]);
	}

	bool passed = false;
	if (token.symbol == InstSymbol_if) passed = expr_passed;
	else if (token.symbol == InstSymbol_elif) passed = expr_passed && not previous_conditional_passed;
	else if (token.symbol == InstSymbol_else) passed = not previous_conditional_passed;

	token.meta = {passed};
	if (token.symbol == InstSymbol_elif && not passed) token.meta[0] = previous_conditional_passed;
	InstTokenSeq[token.i] = token;
	// Jump past instructions in this composite if failed.
	if (not passed) exec_jump_value += token.composite_size;

	// Scope in, if declarative.
	else if (token.declarative_composite){
		scope_in(state);
		scoped_tokens.push_back(&token);
	}
}




const Instruction* INST_If = new Instruction{
	1,             // Required arg count.
	INST_If_exec,  // Function.
	true,          // Is composite.
	true,          // Has expression.
	INST_If_processor,
};
