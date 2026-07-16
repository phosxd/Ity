#pragma once


Variant OP_TypeCast_exec(ScopeState& _state, Variant& first, Variant& second, const std::string& _symbol) {
	if (second.t != STR) {
		emit_error(ERR_operand_type_mismatch, {"TypeCast", get_variant_type_name(first.t), get_variant_type_name(second.t)});
		return first;
	}

	const STR_t& target = std::any_cast<STR_t>(second.d);
	Variant var;

	if (target == "BOOL")        {var.t = BOOL;   var.d = var_to_bool(first);}
	else if (target == "INT")    {var.t = INT;    var.d = var_to_int(first);}
	else if (target == "FLOAT")  {var.t = FLOAT;  var.d = var_to_float(first);}
	else if (target == "STR")    {var.t = STR;    var.d = var_to_str(first);}
	else {
		emit_error(ERR_invalid_cast, {get_variant_type_name(first.t), target});
		return first;
	}

	return var;
}


const Operation OP_TypeCast {
	OP_TypeCast_exec,
};
