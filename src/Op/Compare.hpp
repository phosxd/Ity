#pragma once


Variant OP_Compare_exec(const Operation& op, ScopeState& state, Variant& first, Variant& second, const std::string& symbol) {
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


const Operation OP_Compare {
	OP_Compare_exec,
};
