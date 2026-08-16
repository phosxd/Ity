#pragma once


static void INST_Continue_processor(InstToken& token, const AnyMap_t& extra, const unsigned int& ln, const unsigned int& col) {
	if (token.symbol == InstSymbol_continue || token.symbol == InstSymbol_break) {
		bool found = false;
		std::vector<CompositeItem> reverse_nest = *AnyCast(std::vector<CompositeItem>*,extra.at("cn"));
		std::reverse(reverse_nest.begin(), reverse_nest.end());
		for (const CompositeItem& comp_item : reverse_nest) {
			if (comp_item.token.symbol != InstSymbol_while && comp_item.token.symbol != InstSymbol_for) continue;
			found = true;
			token.linked_inst = comp_item.token.symbol;
			token.linked_inst_pos = -(int32_t)(token.i - comp_item.token.i);
			break;
		}

		// Throw error if there is no loop token to link to.
		if (not found) {
			emit_error(ERR_unexpected_inst, {token.args[0]}, ln,col);
			return;
		}
	}
}


static void INST_Continue_exec(ItyState& state, InstToken& token) {
	// If loop token...
	if (token.linked_inst == InstSymbol_while || token.linked_inst == InstSymbol_for) {
		InstToken& linked_token = state.seq[token.i + token.linked_inst_pos];

		// Jump over the "end" instruction for this loop.
		exec_jump_value += linked_token.composite_size + token.linked_inst_pos;

		// If is "continue", jump to the end instruction, not over it.
		if (token.symbol == InstSymbol_continue) exec_jump_value -= 1;
		// If is "break", properly scope out.
		else {
			// Scope out if previously scoped in.
			if (linked_token.declarative_composite && linked_token.meta.size() > 0) {
				state.scope.out();
				InstToken* linked_token_ptr = &linked_token;
				linked_token.inst->emergency_scope_exit(linked_token_ptr);
				scoped_tokens.pop_back();
			}
		}
	}
}



const Instruction* INST_Continue = new Instruction{
	.REQUIRED = 1,
	.exec = INST_Continue_exec,
	.processor = INST_Continue_processor
};
