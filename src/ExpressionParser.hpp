#pragma once



std::string get_variant_data_type(VariantData& d) {
	if (std::holds_alternative<bool>(d)) {
		return "BOOL";
	}
	else if (std::holds_alternative<int>(d)) {
		return "INT";
	}
	else if (std::holds_alternative<float>(d)) {
		return "FLOAT";
	}
	else if (std::holds_alternative<std::string>(d)) {
		return "STR";
	}
	return "NULL";
}
