#pragma once


Variant OP_Access_exec(const Operation& op, ScopeState& state, Variant& first, Variant& second, std::string& symbol) {
	if (first.t == ARR) {
		if (second.t != INT) {
			emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
			return first;
		}
		const std::vector<Variant>& array = std::any_cast<std::vector<Variant>>(first.d);
		const int& index = std::any_cast<int>(second.d);
		if (index >= array.size()) {
			emit_error(ERR_index_out_of_range, {std::to_string(index)});
			return first;
		}
		return array.at(index);
	}


	// Access object property.
	// For hash tables, functions, or other objects.
	else if (first.t == MAP) {
		const std::unordered_map<std::string,Variant>& map = std::any_cast<std::unordered_map<std::string,Variant>>(first.d);

		// Determine type of the object.
		std::string obj_type = "m";
		if (map.find("__t") != map.end()) {
			const Variant& obj_type_var = map.at("__t");
			if (obj_type_var.t != STR) {
				emit_error(ERR_unexpected, {"OP_Access.exec", "improper type of \"__t\" property."});
				return first;
			}
			obj_type = std::any_cast<std::string>(obj_type_var.d);
		}

		// Access map.
		if (obj_type == "m") {
			// Throw error if accessor is not a string.
			if (second.t != STR) {
				emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
				return first;
			}
			// Find key.
			const std::string& key = std::any_cast<std::string>(second.d);
			if (map.find(key) == map.end()) {
				emit_error(ERR_no_property_with_name, {key});
				return first;
			}
			// Return Variant at the key.
			return map.at(key);
		}

		// Access function.
		else if (obj_type == "f") {
			// Throw error if accessor is not an array.
			if (second.t != ARR) {
				emit_error(ERR_invalid_func_call, {get_variant_type_name(second.t)});
				return first;
			}
			const int func_token_index = std::any_cast<int>(map.at("__idx").d);
			const InstToken& func_token = InstTokenSeq.at(func_token_index);
			const unsigned int func_body_start = func_token.i+1;
			const unsigned int func_body_end = std::any_cast<unsigned int>(func_token.meta.at(0));
			// Run function.
			std::vector<InstToken> inst_token_seq (InstTokenSeq.begin()+func_body_start, InstTokenSeq.begin()+func_body_end);
			scope_in(state);                                                  // Create new scope on top of the previous.
			set_data(state, "_ARGS", ARR, second.d, VariantMode_constant);    // Make the passed arguments available in the scope.
			std::cout << "SCOPE: " << state << '\n';
			Ity.exec(inst_token_seq, state);                                  // Execute instructions in the function.
			scope_out(state);                                                 // Return to previous scope.
		}

	}
	return first;
}


const Operation OP_Access {
	OP_Access_exec,
};
