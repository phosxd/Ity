#pragma once


void OP_Arith_exec(ScopeState& state, Variant*& first, Variant*& second, const std::string& symbol, Variant& result, Variant*& _result_ptr) {
	// All logic for actually getting the results of an arithmetic operation are in `Common.hpp`.
	Variant* o1 = resovlve_potential_ref(state, first);
	Variant* o2 = resovlve_potential_ref(state, second);
	if (symbol == "+")      result = *o1 + *o2;
	else if (symbol == "-") result = *o1 - *o2;
	else if (symbol == "*") result = *o1 * *o2;
	else if (symbol == "/") result = *o1 / *o2;
	else if (symbol == "%") result = *o1 % *o2;
}


const Operation OP_Arith {
	OP_Arith_exec,
};
