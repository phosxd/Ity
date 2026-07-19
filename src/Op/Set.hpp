#pragma once


void OP_Set_exec(ScopeState& state, Variant& first, Variant& second, const std::string& symbol, Variant& result, Variant*& result_ptr) {
	if (first.m == VariantMode_constant) {
		emit_error(ERR_custom, {"Test"});
		return;
	}
	first.t = second.t;

	if (symbol == "=") first.d = second.d;
	else if (symbol == "+=") first.d = first.d + second.d;
	else if (symbol == "-=") first.d = first.d - second.d;
	else if (symbol == "*=") first.d = first.d * second.d;
	else if (symbol == "/=") first.d = first.d / second.d;
	else if (symbol == "%=") first.d = first.d % second.d;

	result_ptr = &first;
}


const Operation OP_Set {
	OP_Set_exec,
};
