#pragma once


void INST_End_exec(ScopeState& state, const Instruction* _inst, InstToken& token, const std::vector<std::string>& args) {
	if (token.linked_inst.empty()) {return;}

	if (token.linked_inst == "while") {
		// Jump back to the while instruction. If it's condition failed, it should jump past this end instruction.
		exec_jump_value += token.linked_inst_pos-1;
		scope_flush(state); // Flush scope for next loop iteration.
	}
	else if (token.linked_inst == "if" || token.linked_inst == "elif" || token.linked_inst == "else") {
		scope_out(state); // Scope out of conditional block.
		scoped_tokens.pop_back();
	}
}


const Instruction INST_End {
	0,              // Required arg count.
	0,              // Optional arg count.
	INST_End_exec,  // Function.
	false,          // Is composite.
};
