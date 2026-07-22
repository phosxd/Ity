#pragma once


void OP_Access_exec(ScopeState& state, Variant& first, Variant& second, const std::string& _symbol, Variant& result, Variant*& result_ptr) {
	// if (second.t == STR) {
	// 	get_data_globally(state, "__candi");
	// }


	// Access array element.
	if (first.t == ARR) {
		if (second.t != INT) {
			emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
			return;
		}
		ARR_t& array = AnyCastV(ARR_t,first.d);
		const INT_t& array_len = (INT_t)array.size();
		INT_t index = AnyCastV(INT_t,second.d);
		// Parse negative index.
		if (index < 0) index = array_len+index;
		// Throw error if index out of range.
		if (index >= array_len || index < 0) {
			emit_error(ERR_index_out_of_range, {std::to_string(AnyCast(INT_t,second.d))}); // Use original given index for error to reduce confusion in negative index cases.
			return;
		}
		// Return reference to array element.
		result_ptr = &array[index];
		return;
	}

	// Access string character.
	else if (first.t == STR) {
		if (second.t != INT) {
			emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
			return;
		}
		const STR_t& str = AnyCast(STR_t,first.d);
		const INT_t& index = AnyCast(INT_t,second.d);
		if (index >= (INT_t)str.size()) {
			emit_error(ERR_index_out_of_range, {std::to_string(index)});
			return;
		}
		result = Variant{STR, std::string(1,str[index])};
		return;
	}


	// Access object property.
	// For hash tables, functions, or other objects.
	else if (first.t == MAP) {
		MAP_t& map = AnyCastV(MAP_t,first.d);

		// Determine type of the object.
		STR_t obj_type = "m";
		if (map.find("__t") != map.end()) {
			const Variant& obj_type_var = map.at("__t");
			if (obj_type_var.t != STR) {
				emit_error(ERR_unexpected, {"OP_Access.exec", "Improper type of \"__t\" property."});
				return;
			}
			obj_type = AnyCast(STR_t,obj_type_var.d);
		}

		// Access map.
		if (obj_type == "m") {
			// Throw error if accessor is not a string.
			if (second.t != STR) {
				emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
				return;
			}
			// Find key.
			const STR_t& key = AnyCast(STR_t,second.d);
			const auto& it = map.find(key);
			if (it == map.end()) {
				emit_error(ERR_no_property_with_name, {key});
				return;
			}
			// Return Variant at the key.
			result_ptr = &it->second;
			return;
		}

		// Access function.
		else if (obj_type == "f") {
			// Throw error if accessor is not an array.
			if (second.t != ARR) {
				emit_error(ERR_invalid_func_call, {get_variant_type_name(second.t)});
				return;
			}
			// Call native function...
			if (const auto& it = map.find("__nc"); it != map.end()) {
				const NativeFunc_t& n_func = AnyCast(NativeFunc_t,it->second.d);
				const ARR_t& n_args = AnyCast(ARR_t,second.d);
				result = n_func(state, n_args);
				return;
			}
			// Call script function...
			else {
				result = call_script_function(state, map, second);
				return;
			}
		}

	}
	emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
}


const Operation OP_Access {
	OP_Access_exec,
};
