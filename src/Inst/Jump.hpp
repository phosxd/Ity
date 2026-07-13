#pragma once


void INST_Jump_exec(const Instruction* _inst, InstToken& _token, ScopeState& _state, const std::vector<std::string>& args) {
	const unsigned int args_len = args.size();
	int jump_count = 0;
	for (unsigned int i = 1; i < args_len; i++) {
		if (i == 1) {
			if (args[i].size() > 0 && (NUM.find(args[i]) != std::string::npos || args[i][0] == '-')) {
				jump_count = std::stoi(args[i]);
			}
		}
	}

	if (jump_count == 0) {return;}
	jump_count -= 1; // Exclude the current instruction from count.
	// Set jump value.
	exec_jump_value = jump_count;
}


const Instruction INST_Jump {
	1,              // Required arg count.
	0,              // Optional arg count.
	INST_Jump_exec, // Function.
	false,          // Is composite.
};
