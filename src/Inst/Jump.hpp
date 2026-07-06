#pragma once


void INST_Jump_exec(const Instruction& inst, const InstToken& token, ScopeState& state, const std::vector<std::string>& args, const std::string& symbol) {
	const unsigned int args_len = args.size();
	int jump_count = 0;
	for (unsigned int i = 0; i < args_len; i++) {
		if (i == 0) {continue;}
		if (i == 1) {
			if (args[i].size() > 0 && (NUM.find(args[i]) != std::string::npos || args[i][0] == '-')) {
				jump_count = std::stoi(args[i]);
			}
		}
	}

	if (jump_count == 0) {return;}
	exec_jump_value = jump_count;
}


Instruction INST_Jump {
	1, // "REQUIRED". Jump Count.
	0,
	INST_Jump_exec,
};
