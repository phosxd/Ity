#pragma once


VariantData get_literal_from_str(const VariantType& type, const std::string& str_val) {
	if (type == OP || type == REF || type == STR) {return str_val;}
	else if (type == BOOL) {return (str_val == "true");}
	else if (type == INT) {
		if (not is_int_str_32_in_range(str_val)) {
			emit_error(ERR_cannot_initialize_value, {str_val, "Number too large"});
			return std::any();
		}
		return std::stoi(str_val);
	}
	else if (type == FLOAT) {return std::stof(str_val);}
	else {return std::any();}
}


Variant OP_TypeCast_exec(Variant& first, Variant& second, const std::string& _symbol) {
	if (second.t != STR) {
		emit_error(ERR_operand_type_mismatch, {"TypeCast", get_variant_type_name(first.t), get_variant_type_name(second.t)});
		return first;
	}
	const STR_t target = std::any_cast<STR_t>(second.d);

	if (first.t == STR) {
		const STR_t& data = std::any_cast<STR_t>(first.d);
		if (target == "STR") return first;
		else if (target == "BOOL") return Variant{BOOL, get_literal_from_str(BOOL,data)};
		else if (target == "INT") return Variant{INT, get_literal_from_str(INT,data)};
		else if (target == "FLOAT") return Variant{FLOAT, get_literal_from_str(FLOAT,data)};
	}
	else if (first.t == INT) {
		const int& data = std::any_cast<int>(first.d);
		if (target == "INT") return first;
		else if (target == "BOOL") return Variant{BOOL, data>0};
		else if (target == "FLOAT") return Variant{FLOAT, (float)data};
		else if (target == "STR") return Variant{STR, std::to_string(data)};
	}
	else if (first.t == FLOAT) {
		const float& data = std::any_cast<float>(first.d);
		if (target == "FLOAT") return first;
		else if (target == "BOOL") return Variant{BOOL, data>0};
		else if (target == "INT") return Variant{INT, (int)data};
		else if (target == "STR") return Variant{STR, std::to_string(data)};
	}
	emit_error(ERR_invalid_cast, {get_variant_type_name(first.t), target});
	return first;
}


const Operation OP_TypeCast {
	OP_TypeCast_exec,
};
