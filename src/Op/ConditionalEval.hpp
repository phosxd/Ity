#pragma once


Variant OP_ConditionalEval_pre_exec(Variant& first, const std::string& symbol) {
	if (symbol == "&&") {
		eval_second_operand = first.d == true;
		return VariantPresets.bool_false;
	}
	else if (symbol == "||") {
		eval_second_operand = first.d == false;
		return VariantPresets.bool_true;
	}

	return VariantPresets.empty;
}


Variant OP_ConditionalEval_exec(Variant& first, Variant& second, const std::string& symbol) {
	bool test_result = false;
	if (symbol == "&&") test_result = first.d == true && second.d == true;
	else if (symbol == "||") test_result = first.d == true || second.d == true;

	return Variant {
		BOOL,
		test_result,
	};
}


const Operation OP_ConditionalEval {
	OP_ConditionalEval_exec,
	OP_ConditionalEval_pre_exec,
};
