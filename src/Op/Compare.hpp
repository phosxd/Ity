#pragma once


Variant OP_Compare_exec(const Operation& op, ScopeState& state, Variant& first, Variant& second, std::string symbol) {
	AccountNegativeSymbol(second, symbol);

	bool test_result = false;
	if (symbol == "==") {test_result = (first.d == second.d);}
	else if (symbol == "!=") {test_result = not (first.d == second.d);}
	else if (symbol == ">") {test_result = (first.d > second.d);}
	else if (symbol == "<") {test_result =(first.d < second.d);}
	else if (symbol == ">=") {test_result = (first.d == second.d || first.d > second.d);}
	else if (symbol == "<=") {test_result  = (first.d == second.d || first.d < second.d);}
	return Variant {
		BOOL,
		test_result,
	};
}


Operation OP_Compare {
	OP_Compare_exec,
};
