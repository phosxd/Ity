#pragma once


void OP_TypeCast_exec(ScopeState& _state, Variant*& first, Variant*& second, const OpSymbol& _symbol, Variant& result, Variant*& _result_ptr) {
	if (second->t != INT) {
		emit_error(ERR_operand_type_mismatch, {"TypeCast", get_variant_type_name(first->t), get_variant_type_name(second->t)});
		return;
	}

	const INT_t& target = AnyCast(INT_t,second->d);
	Variant var;

	switch ((VariantType)target) {
		case BOOL:   {var.t = BOOL;   var.d = var_to_bool(*first);  break;}
		case INT:    {var.t = INT;    var.d = var_to_int(*first);   break;}
		case FLOAT:  {var.t = FLOAT;  var.d = var_to_float(*first); break;}
		case STR:    {var.t = STR;    var.d = var_to_str(*first);   break;}
		default: {
			emit_error(ERR_invalid_cast, {get_variant_type_name(first->t), get_variant_type_name((VariantType)target)});
			return;
		}
	}

	result = var;
}




const auto OP_TypeCast = new Operation{
	OP_TypeCast_exec,
};
