#pragma once


Variant OP_Arith_exec(Operation& op, ScopeState& state, Variant& first, Variant& second, std::string symbol) {
	// Fixer for negative numbers.
	if (first.t == NONE && (second.t == INT || second.t == FLOAT)) {
		second.d = second.d-(second.d*2);
		return Variant {
			second.t,
			second.d,
		};
	}
	AccountNegativeSymbol(second, symbol);

	VariantType type = first.t;
	// Convert output type to FLOAT if either operands are of type FLOAT.
	if ((first.t == INT && second.t == FLOAT) || (first.t == FLOAT && second.t == INT)) {
		type = FLOAT;
	}

	VariantData result;
	if (symbol == "+") {result = first.d + second.d;}
	else if (symbol == "-") {result = first.d - second.d;}
	else if (symbol == "*") {result = first.d * second.d;}
	else if (symbol == "/") {result = first.d / second.d;}
	else if (symbol == "%") {result = first.d % second.d;}
	return Variant {
		type,
		result,
	};
}


Operation OP_Arith {
	OP_Arith_exec,
};
