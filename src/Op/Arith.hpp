#pragma once


void OP_Arith_exec(ScopeState& _state, Variant*& first, Variant*& second, const std::string& symbol, Variant& result, Variant*& _result_ptr) {
	// All logic for actually getting the results of an arithmetic operation are in `Common.hpp`.
	if (symbol == "+")      result = *first + *second;
	else if (symbol == "-") result = *first - *second;
	else if (symbol == "*") result = *first * *second;
	else if (symbol == "/") result = *first / *second;
	else if (symbol == "%") result = *first % *second;
}


const Operation OP_Arith {
	OP_Arith_exec,
};
