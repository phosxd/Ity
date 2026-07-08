#pragma once


// void INST_End_exec(const Instruction& inst, const InstToken& token, ScopeState& state, const std::vector<std::string>& args) {
// 	if (token.linked_inst.empty()) {return;}
//
// 	if (token.linked_inst == "while") {
// 		// Jump back to the while instruction. If it's condition failed, it should jump past this end instruction.
// 		exec_jump_value += token.linked_inst_pos-1;
// 	}
// }


// const Instruction INST_End {
// 	0,              // Required arg count.
// 	0,              // Optional arg count.
// 	INST_End_exec,  // Function.
// 	false,          // Is composite.
// };


const Variant LIB_IO {

}
