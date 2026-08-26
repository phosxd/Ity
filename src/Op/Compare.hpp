#pragma once


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


void OP_Compare_exec(ItyState& state, Variant*& first, Variant*& second, const OpSymbol& symbol, Variant& result, Variant*& _result_ptr) {
	const Variant* o1 = resovlve_potential_ref(state, first);
	const Variant* o2 = resovlve_potential_ref(state, second);

	bool test = false;
	switch (symbol) {
		case OpSymbol_cmp_eq:   {test = *o1 == *o2;        break;}
		case OpSymbol_cmp_neq:  {test = not (*o1 == *o2);  break;}
		case OpSymbol_cmp_gt:   {test = *o1 > *o2;         break;}
		case OpSymbol_cmp_lt:   {test = *o1 < *o2;         break;}
		case OpSymbol_cmp_gteq: {test = *o1 == *o2 || *o1 > *o2; break;}
		case OpSymbol_cmp_lteq: {test = *o1 == *o2 || *o1 < *o2; break;}
		case OpSymbol_cmp_and:  {test = *o1 == VPS.bool_true && *o2 == VPS.bool_true; break;}
		case OpSymbol_cmp_or:   {test = *o1 == VPS.bool_true || *o2 == VPS.bool_true; break;}
		default: break;
	}

	result = Variant{BOOL, std::move(test)};
}




const auto OP_Compare = new Operation{
	OP_Compare_exec,
	OP_Compare_pre_exec,
};
