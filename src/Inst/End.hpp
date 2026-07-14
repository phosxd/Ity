#pragma once


void INST_End_exec(const Instruction* _inst, InstToken& token, const std::vector<std::string>& args) {
	if (token.linked_inst.empty()) {return;}

	if (token.linked_inst == "while") {
		// Jump back to the while instruction. If it's condition failed, it should jump past this end instruction.
		exec_jump_value += token.linked_inst_pos-1;
		scope_flush(ST); // Flush scope for next loop iteration.
	}
}


const Instruction INST_End {
	0,              // Required arg count.
	0,              // Optional arg count.
	INST_End_exec,  // Function.
	false,          // Is composite.
};
