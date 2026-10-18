#pragma once
// Fragment Size: 2004


void OP_Compare_pre_exec(ItyState& state, Variant*& first, const OpSymbol& symbol, bool& eval_second_operand, Variant& result, Variant*& _result_ptr) {
	Variant* o1 = resovlve_potential_ref(state, first);

	if (symbol == OpSymbol_cmp_and) {
		eval_second_operand = *o1 == VPS.bool_true;
		result = VPS.bool_false;
	}
	else if (symbol == OpSymbol_cmp_or) {
		eval_second_operand = *o1 == VPS.bool_false;
		result = VPS.bool_true;
	}
}


void OP_Compare_exec(ItyState& state, ExprState& _expr_state, Variant*& first, Variant*& second, const OpSymbol& symbol, Variant& result, Variant*& _result_ptr) {
	const Variant* o1 = resovlve_potential_ref(state, first);
	const Variant* o2 = resovlve_potential_ref(state, second);

	bool test = false;
	if (symbol == OpSymbol_cmp_eq || symbol == OpSymbol_cmp_neq) {
		test = *o1 == *o2;
		if (symbol == OpSymbol_cmp_neq) test = not test;
	}

	else if (symbol == OpSymbol_cmp_gteq || symbol == OpSymbol_cmp_lteq) test = *o1 == *o2 || ((symbol == OpSymbol_cmp_gteq) ? (*o1 > *o2) : (*o1 < *o2));

	else if (symbol == OpSymbol_cmp_and || symbol == OpSymbol_cmp_or) {
		test = *o1 == VPS.bool_true;
		bool test_tmp = *o2 == VPS.bool_true;
		test = (symbol == OpSymbol_cmp_and) ? (test && test_tmp) : (test || test_tmp);
	}

	else if (symbol == OpSymbol_cmp_gt) test = *o1 > *o2;
	else if (symbol == OpSymbol_cmp_lt) test = *o1 < *o2;

	result = Variant{VT_BOOL, test};
}




const auto OP_Compare = new Operation{
	OP_Compare_exec,
	OP_Compare_pre_exec,
};
