#pragma once


void OP_Compare_exec(ScopeState& _state, Variant& first, Variant& second, const std::string& symbol, Variant& result, Variant*& _result_ptr) {
	bool test_result = false;
	if (symbol == "==")        test_result = (first.d == second.d);
	else if (symbol == "!=")   test_result = not (first.d == second.d);
	else if (symbol == ">")    test_result = (first.d > second.d);
	else if (symbol == "<")    test_result = (first.d < second.d);
	else if (symbol == ">=")   test_result = (first.d == second.d || first.d > second.d);
	else if (symbol == "<=")   test_result = (first.d == second.d || first.d < second.d);

	result = Variant {BOOL, test_result};
}


const Operation OP_Compare {
	OP_Compare_exec,
};
