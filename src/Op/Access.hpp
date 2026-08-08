#pragma once


const bool OP_Access_type_method(const std::string& type_name, const STR_t& method_name, MAP_t& methods, Variant*& o1, Variant& result) {
	MAP_t::iterator it = methods.find((type_name+':'+method_name));

	// If method not found in top level type, try in the MAP type.
	if (o1->t == MAP && it == methods.end()) {
		const STR_t& map_type = var_get_obj_type(AnyCast(MAP_t,o1->d));
		it = methods.find( (type_name+'('+map_type+')'+':'+method_name) );
	}

	// Return the method.
	if (it != methods.end()) {
		MAP_t func = AnyCast(MAP_t,it->second.d); // Copy function.
		func["__ba"].d = AnyCast(ARR_t,func["__ba"].d) + (ARR_t){Variant{PTR, o1}}; // Bind first variant to the function copy.
		// Return copied function.
		result = Variant{MAP, func, VariantMode_constant};
		return true;
	}
	return false;
}




void OP_Access_exec(ScopeState& state, Variant*& first, Variant*& second, const OpSymbol& _symbol, Variant& result, Variant*& result_ptr) {
	Variant* o1 = resovlve_potential_ref(state, first);
	Variant* o2 = resovlve_potential_ref(state, second);


	// Try to access type method.
	if (o2->t == STR) {
		if (Variant* type_methods = get_data_globally(state, "__tm__"); type_methods) {
			// Find & return method.
			MAP_t& methods = AnyCastV(MAP_t,type_methods->d);
			const STR_t& method_name = AnyCast(STR_t,second->d);
			// Try pointer/reference type methods first.
			if (first->t == PTR || first->t == REF) {
				if (OP_Access_type_method(get_variant_type_name(first->t), method_name, methods, first, result)) return;
			}
			// Try.
			if (OP_Access_type_method(get_variant_type_name(o1->t), method_name, methods, o1, result)) return;
		}
	}


	switch (o1->t) {
		// Access array element.
		case ARR: {
			if (o2->t != INT) {
				emit_error(ERR_invalid_property_access, {get_variant_type_name(o1->t), get_variant_type_name(o2->t)});
				return;
			}
			ARR_t& array = AnyCastV(ARR_t,o1->d);
			const INT_t& array_len = (INT_t)array.size();
			INT_t index = AnyCastV(INT_t,o2->d);
			// Parse negative index.
			if (index < 0) index = array_len+index;
			// Throw error if index out of range.
			if (index >= array_len || index < 0) {
				emit_error(ERR_index_out_of_range, {std::to_string(AnyCast(INT_t,o2->d))}); // Use original given index for error to reduce confusion in negative index cases.
				return;
			}
			// Return reference to array element.
			result_ptr = &array[index];
			return;
	}


		// Access string character.
		case STR: {
			if (o2->t != INT) {
				emit_error(ERR_invalid_property_access, {get_variant_type_name(o1->t), get_variant_type_name(o2->t)});
				return;
			}
			const STR_t& str = AnyCast(STR_t,o1->d);
			const INT_t& index = AnyCast(INT_t,o2->d);
			if (index >= (INT_t)str.size()) {
				emit_error(ERR_index_out_of_range, {std::to_string(index)});
				return;
			}
			result = Variant{STR, std::string(1,str[index])};
			return;
	}


		// Access object property.
		// For hash tables, functions, or other objects.
		case MAP: {
			MAP_t& map = AnyCastV(MAP_t,o1->d);
			// Determine type of the object.
			const STR_t& obj_type = var_get_obj_type(map);

			// Access function...
			if (obj_type == "f") {
				// Construct arguments array with user passed & bound arguments.
				const ARR_t args_arr = AnyCast(ARR_t,map.at("__ba").d)
				+ ( (second->t == ARR) ? AnyCast(ARR_t,second->d) : (ARR_t){*second} ); // Using "second" instead of "o2" is not a mistake, if it's a `REF`/`PTR` we don't want to use the referenced value. Pass the actual ref/ptr.
				// Call native function...
				if (const MAP_t::iterator& it = map.find("__nc"); it != map.end()) {
					result = call_native_function(state, AnyCast(NativeFunc_t,it->second.d), args_arr);
				}
				// Call script function...
				else {
					Variant args {ARR, args_arr};
					result = call_script_function(state, map, args);
				}
			}
			// Access as map if type doesn't match any of the above...
			else {
				// Throw error if accessor is not a string.
				if (o2->t != STR) {
					emit_error(ERR_invalid_property_access, {get_variant_type_name(o1->t), get_variant_type_name(o2->t)});
					return;
				}
				// Find key.
				const STR_t& key = AnyCast(STR_t,o2->d);
				const MAP_t::iterator& it = map.find(key);
				if (it == map.end()) {
					emit_error(ERR_no_property_with_name, {key});
					return;
				}
				// Return Variant at the key.
				result_ptr = &it->second;
			}
			return;
		}

		default: break;
	}
	emit_error(ERR_invalid_property_access, {get_variant_type_name(o1->t), get_variant_type_name(o2->t)});
}




const auto OP_Access = new Operation{
	OP_Access_exec,
};
