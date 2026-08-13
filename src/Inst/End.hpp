#pragma once


void INST_End_exec(ItyState& state, InstToken& token) {
	if (token.linked_inst == InstSymbol__) return;

	if (token.linked_inst == InstSymbol_while || token.linked_inst == InstSymbol_for) {
		// Jump back to the while instruction. If it's condition failed, it should jump past this end instruction.
		exec_jump_value += token.linked_inst_pos-1;
		if (token.declarative_composite) state.scope.flush(); // Flush scope for next loop iteration.
	}

	else if (token.declarative_composite && (token.linked_inst == InstSymbol_if || token.linked_inst == InstSymbol_elif || token.linked_inst == InstSymbol_else)) {
		state.scope.out(); // Scope out of conditional block.
		scoped_tokens.pop_back();
	}
}




const Instruction* INST_End = new Instruction{
	1,              // Required arg count.
	INST_End_exec,  // Function.
	false,          // Is composite.
	false,          // Has expression.
};
