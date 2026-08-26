#pragma once


void OP_Ternary_pre_exec(ItyState& state, Variant*& first, const OpSymbol& symbol, bool& eval_second_operand, Variant& result, Variant*& _result_ptr) {
	Variant* o1 = resovlve_potential_ref(state, first);

	if (symbol == OpSymbol_ternary) {
		eval_second_operand = *o1 == VPS.bool_true;
		if (not eval_second_operand) result = Variant{INTERNAL, std::monostate()};
	}
	else {
		eval_second_operand = o1->t == INTERNAL;
	}
}


void OP_Ternary_exec(ItyState& state, Variant*& first, Variant*& second, const OpSymbol& symbol, Variant& _result, Variant*& result_ptr) {
	result_ptr = second;
}




const auto OP_Ternary = new Operation{
	OP_Ternary_exec,
	OP_Ternary_pre_exec,
};
