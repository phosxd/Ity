#pragma once
// Fragment Size: 1152


void OP_TypeCast_exec(ItyState& _state, ExprState& _expr_state, Variant*& first, Variant*& second, const OpSymbol& _symbol, Variant& result, Variant*& _result_ptr) {
	if (second->t != VT_INT) {
		emit_error(ERR_operand_type_mismatch, {"TypeCast", get_variant_type_name(first->t), get_variant_type_name(second->t)});
		return;
	}

	Variant var {.t = (VariantType)AnyCast(INT_t,second->d)};
	switch (var.t) {
		case VT_BOOL:   {var.d = first->to_bool();  break;}
		case VT_INT:    {var.d = first->to_int();   break;}
		case VT_FLOAT:  {var.d = first->to_float(); break;}
		case VT_STR:    {var.d = first->to_str();   break;}
		default: {
			emit_error(ERR_invalid_cast, {get_variant_type_name(first->t), get_variant_type_name(var.t)});
			return;
		}
	}

	result = var;
}




const auto OP_TypeCast = new Operation{
	OP_TypeCast_exec,
};
