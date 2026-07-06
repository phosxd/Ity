#pragma once


void INST_End_exec(const Instruction& inst, const InstToken& token, ScopeState& state, const std::vector<std::string>& args, const std::string& symbol) {
	return;
}


Instruction INST_End {
	0,
	0,
	INST_End_exec,
};
