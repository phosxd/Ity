#pragma once


void INST_End_exec(const Instruction& inst, const InstToken& token, ScopeState& state, const std::vector<std::string>& args, const std::string& symbol) {
	if (token.linked_inst.empty()) {return;}

	if (token.linked_inst == "while") {
		// Jump back to the while instruction. If it's condition failed, it should jump past this end instruction.
		exec_jump_value += token.linked_inst_pos-1;
	}
}


Instruction INST_End {
	0,
	0,
	INST_End_exec,
};
