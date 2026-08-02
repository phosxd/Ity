#pragma once


void OP_Compare_pre_exec(ScopeState& state, Variant*& first, const std::string& symbol, bool& eval_second_operand, Variant& result, Variant*& _result_ptr) {
	Variant* o1 = resovlve_potential_ref(state, first);

	if (symbol == "&&") {
		eval_second_operand = *o1 == VariantPresets.bool_true;
		result = VariantPresets.bool_false;
		return;
	}
	else if (symbol == "||") {
		eval_second_operand = *o1 == VariantPresets.bool_false;
		result = VariantPresets.bool_true;
		return;
	}
}


void OP_Compare_exec(ScopeState& state, Variant*& first, Variant*& second, const std::string& symbol, Variant& result, Variant*& _result_ptr) {
	Variant* o1 = resovlve_potential_ref(state, first);
	Variant* o2 = resovlve_potential_ref(state, second);

	bool test = false;
	if (symbol == "==")       test = *o1 == *o2;
	else if (symbol == "!=")  test = not (*o1 == *o2);
	else if (symbol == ">")   test = *o1 > *o2;
	else if (symbol == "<")   test = *o1 < *o2;
	else if (symbol == ">=")  test = *o1 == *o2 || *o1 > *o2;
	else if (symbol == "<=")  test = *o1 == *o2 || *o1 < *o2;
	else if (symbol == "&&")  test = *o1 == VariantPresets.bool_true && *o2 == VariantPresets.bool_true;
	else if (symbol == "||")  test = *o1 == VariantPresets.bool_true || *o2 == VariantPresets.bool_true;
	result = Variant{BOOL, test};
}


const Operation OP_Compare {
	OP_Compare_exec,
	OP_Compare_pre_exec,
};
