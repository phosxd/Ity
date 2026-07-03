#pragma once

#include <string>

#include "../Common.hpp"
#include "../ScriptErrors.hpp"
#include "../ScopeState.hpp"
#include "../ExpressionParser.hpp"


Variant exec(Operation& op, ScopeState& state, Variant& first, Variant& second, std::string _symbol) {
	if (first.t == "." || second.t == ".") {
		emit_error("Cannot add value of no type.");
		return first;
	}

	if (op.TYPE_MAP.find(first.t) == op.TYPE_MAP.end()) {
		emit_error("Invalid type for \"add\" operation \"" + first.t + "\".");
		return first;
	}

	std::vector<std::string> expected_second_type = op.TYPE_MAP.at(first.t);
	if (exists_in_strvec(expected_second_type, second.t) == false) {
		emit_error(err_operand_type_mismatch("add", first.t, second.t));
		return first;
	}

	std::string type = first.t;
	if ((first.t == "INT" && second.t == "FLOAT") || (first.t == "FLOAT" && second.t == "INT")) {
		type = "FLOAT";
	}

	if (first.t == "INT" && second.t == "INT") {
		return Variant {type, (std::get<int>(first.d) + std::get<int>(second.d))};
	}
	else if (type == "FLOAT") {
		float v1;
		float v2;
		if (first.t == "INT") {v1 = static_cast<float>(std::get<int>(first.d));}
		else if (first.t == "FLOAT") {v1 = std::get<float>(first.d);}
		if (second.t == "INT") {v2 = static_cast<float>(std::get<int>(second.d));}
		else if (second.t == "FLOAT") {v2 = std::get<float>(second.d);}
		return Variant {type, v1+v2};
	}
	else if (type == "STR") {
		return Variant {type, (std::get<std::string>(first.d) + std::get<std::string>(second.d))};
	}
	else {return first;}
}


Operation OP_Add {
	{
		{"INT", {"INT","FLOAT"}},
		{"FLOAT", {"FLOAT","INT"}},
		{"STR", {"STR"}},
	},
	exec,
};
