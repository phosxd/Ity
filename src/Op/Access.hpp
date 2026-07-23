#pragma once


void OP_Access_exec(ScopeState& state, Variant& first, Variant& second, const std::string& _symbol, Variant& result, Variant*& result_ptr) {
	// Try to access type method.
	if (second.t == STR && not is_name_globally_free(state, "__tm__")) {
		// Find & return method.
		MAP_t& methods = AnyCastV(MAP_t,get_data_globally(state, "__tm__")->d);
		const std::string type_name = get_variant_type_name(first.t);
		const STR_t method_name = AnyCast(STR_t,second.d);
		MAP_t::iterator it = methods.find((type_name+':'+method_name));

		// If method not found in top level type, try in the MAP type.
		if (first.t == MAP && it == methods.end()) {
			const STR_t& map_type = var_get_obj_type(AnyCast(MAP_t,first.d));
			it = methods.find( (type_name+'('+map_type+')'+':'+method_name) );
		}

		// Return the method.
		if (it != methods.end()) {

			MAP_t func = AnyCast(MAP_t,it->second.d); // Copy function.
			func["__ba"].d = AnyCast(ARR_t,func["__ba"].d) + (ARR_t){Variant{INTERNAL, &first}}; // Bind first variant to the function copy.
			// Return copied function.
			result = Variant{MAP, func, VariantMode_constant};
			return;
		}
	}


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
		const STR_t& obj_type = var_get_obj_type(map);

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
				const ARR_t& n_args = AnyCast(ARR_t,map.at("__ba").d) + AnyCast(ARR_t,second.d); // Merge bound arguments.
				result = n_func(state, n_args);
				return;
			}
			// Call script function...
			else {
				Variant args {ARR, (AnyCast(ARR_t,map.at("__ba").d) + AnyCast(ARR_t,second.d))}; // Merge bound arguments.
				result = call_script_function(state, map, args);
				return;
			}
		}

	}
	emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
}


const Operation OP_Access {
	OP_Access_exec,
};
