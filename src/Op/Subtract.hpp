#pragma once

#include <string>

#include "../Common.hpp"
#include "../ScriptErrors.hpp"
#include "../ScopeState.hpp"
#include "../ExpressionParser.hpp"


Variant OP_Subtract_exec(Operation& op, ScopeState& state, Variant& first, Variant& second, std::string symbol) {
	if ((symbol == "--" || first.t == NONE)) {second.d = second.d-(second.d*2);}
	if (first.t == NONE && op.type_map.find(second.t) != op.type_map.end()) {
		return Variant {
			second.t,
			second.d,
		};
	}

	CheckOperationExec(op, state, first, second, "subtract");

	VariantType type = first.t;
	// Convert output type to FLOAT if either operands are of type FLOAT.
	if ((first.t == INT && second.t == FLOAT) || (first.t == FLOAT && second.t == INT)) {
		type = FLOAT;
	}

	// Return new variant.
	return Variant {
		type,
		(first.d - second.d),
	};
}


Operation OP_Subtract {
	{
		{INT, {INT,FLOAT}},
		{FLOAT, {FLOAT,INT}},
		{ANY, {ANY,INT,FLOAT}},
	},
	OP_Subtract_exec,
};
