#pragma once


void INST_Continue_exec(ScopeState& state, const Instruction* _inst, InstToken& token, const std::vector<std::string>& args) {
	if (token.linked_inst == "while") {
		InstToken& linked_token = InstTokenSeq[token.i + token.linked_inst_pos];
		exec_jump_value += linked_token.composite_size + token.linked_inst_pos;
		if (args[0] == "continue") exec_jump_value -= 1;
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
};
