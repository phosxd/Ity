#pragma once


void OP_Compare_pre_exec(ScopeState& _state, Variant*& first, const std::string& symbol, bool& eval_second_operand, Variant& result, Variant*& _result_ptr) {
	if (symbol == "&&") {
		eval_second_operand = first->d == true;
		result = VariantPresets.bool_false;
		return;
	}
	else if (symbol == "||") {
		eval_second_operand = first->d == false;
		result = VariantPresets.bool_true;
		return;
	}
}


void OP_Compare_exec(ScopeState& _state, Variant*& first, Variant*& second, const std::string& symbol, Variant& result, Variant*& _result_ptr) {
	bool test_result = false;
	if (symbol == "==")        test_result = (first->d == second->d);
	else if (symbol == "!=")   test_result = not (first->d == second->d);
	else if (symbol == ">")    test_result = (first->d > second->d);
	else if (symbol == "<")    test_result = (first->d < second->d);
	else if (symbol == ">=")   test_result = (first->d == second->d || first->d > second->d);
	else if (symbol == "<=")   test_result = (first->d == second->d || first->d < second->d);
	else if (symbol == "&&") test_result = first->d == true && second->d == true;
	else if (symbol == "||") test_result = first->d == true || second->d == true;

	result = Variant {BOOL, test_result};
}


const Operation OP_Compare {
	OP_Compare_exec,
	OP_Compare_pre_exec,
};
