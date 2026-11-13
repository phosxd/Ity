#pragma once
// Fragment Size: 1152


void OP_TypeCast_exec(ItyState& _state, ExprState& _expr_state, Variant*& first, Variant*& second, const OpSymbol& _symbol, Variant& result, Variant*& _result_ptr) {
	if (not type_is_number(second->t)) {
		emit_error(ERR_operand_type_mismatch, {"TypeCast", get_variant_type_name(first->t), get_variant_type_name(second->t)});
		return;
	}

	Variant v = *first;
	v.make_into_type((VariantType)second->to_uint8());
	result = std::move(v);
}




const auto OP_TypeCast = new Operation{
	OP_TypeCast_exec,
};
