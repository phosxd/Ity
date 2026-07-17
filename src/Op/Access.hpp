#pragma once


Variant OP_Access_exec(ScopeState& state, Variant& first, Variant& second, const std::string& _symbol) {
	if (first.t == ARR) {
		if (second.t != INT) {
			emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
			return first;
		}
		const ARR_t& array = std::any_cast<const ARR_t&>(first.d);
		const size_t index = std::any_cast<int>(second.d);
		if (index >= array.size()) {
			emit_error(ERR_index_out_of_range, {std::to_string(index)});
			return first;
		}
		return array[index];
	}

	// Access string character.
	else if (first.t == STR) {
		if (second.t != INT) {
			emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
			return first;
		}
		const STR_t& str = std::any_cast<const STR_t&>(first.d);
		const size_t index = std::any_cast<int>(second.d);
		if (index >= str.size()) {
			emit_error(ERR_index_out_of_range, {std::to_string(index)});
			return first;
		}
		return Variant{STR, std::string(1,str[index])};
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
				emit_error(ERR_unexpected, {"OP_Access.exec", "Improper type of \"__t\" property."});
				return first;
			}
			obj_type = std::move(std::any_cast<STR_t>(obj_type_var.d));
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
			return std::move(map.at(key));
		}

		// Access function.
		else if (obj_type == "f") {
			// Throw error if accessor is not an array.
			if (second.t != ARR) {
				emit_error(ERR_invalid_func_call, {get_variant_type_name(second.t)});
				return first;
			}
			// Call native function...
			if (map.find("__nc") != map.end()) {
				const NativeFunc_t& n_func = std::any_cast<NativeFunc_t>( map.at("__nc").d );
				const ARR_t& n_args = std::any_cast<ARR_t>(second.d);
				return n_func(state, n_args);
			}


			// Call script function...
			else {
				// Throw error if maximum execution depth is reached.
				if (execution_depth > execution_depth_max) {
					emit_error(ERR_max_execution_depth, {std::to_string(execution_depth_max)});
					return first;
				}
				func_arg_index = 0;
				const int& func_token_index = std::any_cast<int>(map.at("__i").d);
				const VariantType func_return_type = get_variant_type_from_name(std::any_cast<STR_t>( map.at("__rt").d ));
				const int& func_scope_depth = std::any_cast<int>(map.at("__si").d);
				const InstToken& func_token = InstTokenSeq.at(func_token_index);

				// Throw error if token is not a function token.
				// The only time this should happen is if the tokens are corrupted in some way.
				if (func_token.args[0] != "func") {
					emit_error(ERR_unexpected, {"OP_Access.exec(ScriptFunc)", "Missing function: " + std::to_string(func_token_index) + " isn't a \"func\" token."});
					return first;
				}

				const unsigned int func_body_start = func_token.i+1;
				const unsigned int& func_body_end = std::any_cast<unsigned int>( func_token.meta.at(0) );
				// Run function.
				call_trace.push_back(current_line); call_trace.push_back(current_column);
				push_back_ongoing_scopes();
				ScopeState func_state = create_new_scope_state(
					(MAP_t){
						{"__ARGS__", Variant{ARR, second.d, VariantMode_constant}},
						{"__RET__", Variant{func_return_type, std::any(), VariantMode_dynamic_type}}
					},
					get_state_at_depth(state, func_scope_depth)
				);
				if (debug_flags.scoping) std::cout << ANSI::orange << "New Alt Scope From: " << func_token.args[2] << "\n" << ANSI::reset;
				Ity.exec(func_state, InstTokenSeq, func_body_start, func_body_end);
				restore_ongoing_scopes();

				// Get result & check if return type matches.
				const Variant& result = get_data(func_state, "__RET__");
				if (result.t != func_return_type && func_return_type != ANY) {
					emit_error(ERR_return_type_mismatch, {get_variant_type_name(result.t), get_variant_type_name(func_return_type)});
				}
				// Restore previous scope, then return result.
				call_trace.pop_back(); call_trace.pop_back();
				if (debug_flags.scoping) std::cout << ANSI::orange << "Destroyed Alt Scope From: " << func_token.args[2] << " \n" << ANSI::reset;
				return std::move(result);
			}
		}

	}
	emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
	return first;
}


const Operation OP_Access {
	OP_Access_exec,
};
