#pragma once

#include <string>

#include "../Common.hpp"
#include "../ScriptErrors.hpp"
#include "../ScopeState.hpp"
#include "../ExpressionParser.hpp"


Variant OP_Add_exec(Operation& op, ScopeState& state, Variant& first, Variant& second, std::string symbol) {
	CheckOperationExec(op, state, first, second, "add");

	VariantType type = first.t;
	// Convert output type to FLOAT if either operands are of type FLOAT.
	if ((first.t == INT && second.t == FLOAT) || (first.t == FLOAT && second.t == INT)) {
		type = FLOAT;
	}

	// Return new variant.
	if (symbol == "+-") {second.d = second.d-(second.d*2);}
	return Variant {
		type,
		(first.d + second.d),
	};
}


Operation OP_Add {
	{
		{INT, {INT,FLOAT}},
		{FLOAT, {FLOAT,INT}},
		{STR, {STR}},
		{ANY, {ANY,INT,FLOAT,STR}},
	},
	OP_Add_exec,
};
