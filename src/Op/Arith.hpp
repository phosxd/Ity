#pragma once


void OP_Arith_exec(ScopeState& _state, Variant& first, Variant& second, const std::string& symbol, Variant& result, Variant*& _result_ptr) {
	VariantType type = first.t;
	// Convert output type to FLOAT if either operands are of type FLOAT.
	if ((first.t == INT && second.t == FLOAT) || (first.t == FLOAT && second.t == INT)) {
		type = FLOAT;
	}

	VariantData data;
	if (symbol == "+") data = first.d + second.d;
	else if (symbol == "-") data = first.d - second.d;
	else if (symbol == "*") data = first.d * second.d;
	else if (symbol == "/") data = first.d / second.d;
	else if (symbol == "%") data = first.d % second.d;

	result = Variant {type, data};
}


const Operation OP_Arith {
	OP_Arith_exec,
};
