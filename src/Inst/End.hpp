#pragma once


void INST_End_exec(ScopeState& state, InstToken& token) {
	if (token.linked_inst == InstSymbol__) return;

	if (token.linked_inst == InstSymbol_while || token.linked_inst == InstSymbol_for) {
		// Jump back to the while instruction. If it's condition failed, it should jump past this end instruction.
		exec_jump_value += token.linked_inst_pos-1;
		if (token.declarative_composite) scope_flush(state); // Flush scope for next loop iteration.
	}

	else if (token.declarative_composite && (token.linked_inst == InstSymbol_if || token.linked_inst == InstSymbol_elif || token.linked_inst == InstSymbol_else)) {
		scope_out(state); // Scope out of conditional block.
		scoped_tokens.pop_back();
	}
}




const Instruction* INST_End = new Instruction{
	// Valid symbols.
	{InstSymbol_end},
	1,              // Required arg count.
	INST_End_exec,  // Function.
	false,          // Is composite.
	false,          // Has expression.
};
