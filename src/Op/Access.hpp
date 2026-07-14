#pragma once


Variant OP_Access_exec(const Operation& op, ScopeState& state, Variant& first, Variant& second, const std::string& _symbol) {
	if (first.t == ARR) {
		if (second.t != INT) {
			emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
			return first;
		}
		const ARR_t& array = std::any_cast<ARR_t>(first.d);
		const long unsigned int& index = std::any_cast<int>(second.d);
		if (index >= array.size()) {
			emit_error(ERR_index_out_of_range, {std::to_string(index)});
			return first;
		}
		return array.at(index);
	}

	// Access string character.
	else if (first.t == STR) {
		if (second.t != INT) {
			emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
			return first;
		}
		const STR_t& str = std::any_cast<STR_t>(first.d);
		const long unsigned int& index = std::any_cast<int>(second.d);
		if (index >= str.size()) {
			emit_error(ERR_index_out_of_range, {std::to_string(index)});
			return first;
		}
		return Variant{STR, std::string(1,str.at(index))};
	}


	// Access object property.
	// For hash tables, functions, or other objects.
	else if (first.t == MAP) {
		const MAP_t& map = std::any_cast<MAP_t>(first.d);

		// Determine type of the object.
		STR_t obj_type = "m";
		if (map.find("__t") != map.end()) {
			const Variant& obj_type_var = map.at("__t");
			if (obj_type_var.t != STR) {
				emit_error(ERR_unexpected, {"OP_Access.exec", "improper type of \"__t\" property."});
				return first;
			}
			obj_type = std::any_cast<STR_t>(obj_type_var.d);
		}

		// Access map.
		if (obj_type == "m") {
			// Throw error if accessor is not a string.
			if (second.t != STR) {
				emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
				return first;
			}
			// Find key.
			const STR_t& key = std::any_cast<STR_t>(second.d);
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
			// Call native function...
			if (map.find("__ncall") != map.end()) {
				const NativeFunc_t& n_func = std::any_cast<NativeFunc_t>(map.at("__ncall").d);
				const ARR_t& n_args = std::any_cast<ARR_t>(second.d);
				return n_func(state, n_args);
			}

			// Call in-code function...
			else {
				// Throw error if maximum execution depth is reached.
				if (execution_depth > execution_depth_max) {
					emit_error(ERR_max_execution_depth, {std::to_string(execution_depth_max)});
					return first;
				}
				func_arg_index = 0;
				const int& func_token_index = std::any_cast<int>(map.at("__idx").d);
				const VariantType& func_return_type = get_variant_type_from_name(std::any_cast<STR_t>(map.at("__ret_t").d));
				const InstToken& func_token = InstTokenSeq.at(func_token_index);
				const unsigned int func_body_start = func_token.i+1;
				const unsigned int& func_body_end = std::any_cast<unsigned int>(func_token.meta.at(0));
				// Run function.
				scope_in(state);                                                        // Create new scope on top of the previous.
				set_data(state, "__ARGS__", ARR, second.d, VariantMode_constant);       // Make the passed arguments available in the scope.
				set_data(state, "__RET__", ANY, std::any(), VariantMode_dynamic_type);  // Initialize return variable.
				Ity.exec(InstTokenSeq, state, func_body_start, func_body_end);

				// Get result & check if return type matches.
				const Variant& result = get_data(state, "__RET__");
				if (result.t != func_return_type) {
					emit_error(ERR_return_type_mismatch, {get_variant_type_name(func_return_type), get_variant_type_name(result.t)});
				}
				// Restore previous scope, then return result.
				scope_out(state);
				return result;
			}
		}

	}
	return first;
}


const Operation OP_Access {
	OP_Access_exec,
};
