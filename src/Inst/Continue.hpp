#pragma once


void INST_Continue_processor(const Instruction* _inst, InstToken& token, const AnyMap_t& extra, const unsigned int& ln, const unsigned int& col) {
	if (token.args[0] == "continue" || token.args[0] == "break") {
		bool found = false;
		std::vector<CompositeItem> reverse_nest = *std::any_cast<std::vector<CompositeItem>*>(extra.at("composite_nest"));
		std::reverse(reverse_nest.begin(), reverse_nest.end());
		for (const CompositeItem& comp_item : reverse_nest) {
			if (comp_item.token.args[0] != "while" && comp_item.token.args[0] != "for") continue;
			found = true;
			token.linked_inst = comp_item.token.args[0];
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


void INST_Continue_exec(ScopeState& state, const Instruction* _inst, InstToken& token) {
	if (token.linked_inst == "while" || token.linked_inst == "for") {
		InstToken& linked_token = InstTokenSeq[token.i + token.linked_inst_pos];
		exec_jump_value += linked_token.composite_size + token.linked_inst_pos;
		if (token.args[0] == "continue") exec_jump_value -= 1;
		else {
			// Scope out if previously scoped in.
			if (linked_token.declarative_composite && linked_token.meta.size() > 0) {
				scope_out(state);
				linked_token.meta.clear();
				scoped_tokens.pop_back();
			}
		}
	}
}


const Instruction INST_Continue {
	1,                   // Required arg count.
	INST_Continue_exec,  // Function.
	false,               // Is composite.
	false,               // Has expression.
	INST_Continue_processor,
};
