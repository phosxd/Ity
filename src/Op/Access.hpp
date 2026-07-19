#pragma once


void OP_Access_exec(ScopeState& state, Variant& first, Variant& second, const std::string& _symbol, Variant& result, Variant*& result_ptr) {
	if (first.t == ARR) {
		if (second.t != INT) {
			emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
			return;
		}
		ARR_t& array = std::any_cast<ARR_t&>(first.d);
		const INT_t& index = std::any_cast<const INT_t&>(second.d);
		if (index >= (INT_t)array.size()) {
			emit_error(ERR_index_out_of_range, {std::to_string(index)});
			return;
		}
		result_ptr = &array[index];
		return;
	}

	// Access string character.
	else if (first.t == STR) {
		if (second.t != INT) {
			emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
			return;
		}
		const STR_t& str = std::any_cast<const STR_t&>(first.d);
		const INT_t& index = std::any_cast<const INT_t&>(second.d);
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
		MAP_t& map = std::any_cast<MAP_t&>(first.d);

		// Determine type of the object.
		STR_t obj_type = "m";
		if (map.find("__t") != map.end()) {
			const Variant& obj_type_var = map.at("__t");
			if (obj_type_var.t != STR) {
				emit_error(ERR_unexpected, {"OP_Access.exec", "Improper type of \"__t\" property."});
				return;
			}
			obj_type = std::any_cast<STR_t>(obj_type_var.d);
		}

		// Access map.
		if (obj_type == "m") {
			// Throw error if accessor is not a string.
			if (second.t != STR) {
				emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
				return;
			}
			// Find key.
			const STR_t& key = std::any_cast<STR_t>(second.d);
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
				const NativeFunc_t& n_func = std::any_cast<NativeFunc_t>( it->second.d );
				const ARR_t& n_args = std::any_cast<const ARR_t&>(second.d);
				result = n_func(state, n_args);
				return;
			}


			// Call script function...
			else {
				// Throw error if maximum execution depth is reached.
				if (execution_depth > execution_depth_max) {
					emit_error(ERR_max_execution_depth, {std::to_string(execution_depth_max)});
					return;
				}
				func_arg_index = 0;
				const INT_t& func_token_index = std::any_cast<const INT_t&>(map.at("__i").d);
				const VariantType& func_return_type = std::any_cast<const VariantType&>( map.at("__rt").d );
				const INT_t& func_scope_depth = std::any_cast<const INT_t&>(map.at("__si").d);
				const InstToken& func_token = InstTokenSeq.at(func_token_index);

				// Throw error if token is not a function token.
				// The only time this should happen is if the tokens are corrupted in some way.
				if (func_token.args[0] != "func") {
					emit_error(ERR_unexpected, {"OP_Access.exec(ScriptFunc)", "Missing function: " + std::to_string(func_token_index) + " isn't a \"func\" token."});
					return;
				}

				const unsigned int func_body_start = func_token.i+1;
				const unsigned int& func_body_end = std::any_cast<const unsigned int&>( func_token.meta.at(0) );
				// Run function.
				call_trace.push_back(current_line); call_trace.push_back(current_column);
				push_back_ongoing_scopes();
				ScopeState func_state = create_new_scope_state(
					(MAP_t){
						{"__ARGS__", second},
						{"__RET__", Variant{func_return_type, std::any(), VariantMode_dynamic_type}}
					},
					get_state_at_depth(state, func_scope_depth)
				);
				if (debug_flags.scoping) std::cout << ANSI::orange << "New Alt Scope From: " << func_token.args[2] << "\n" << ANSI::reset;
				Ity::exec(func_state, InstTokenSeq, func_body_start, func_body_end);
				restore_ongoing_scopes();

				// Get result & check if return type matches.
				Variant* func_result = get_data(func_state, "__RET__");
				if (func_result->t != func_return_type && func_return_type != ANY) {
					emit_error(ERR_return_type_mismatch, {get_variant_type_name(func_result->t), get_variant_type_name(func_return_type)});
				}
				// Restore previous scope, then return result.
				call_trace.pop_back(); call_trace.pop_back();
				if (debug_flags.scoping) std::cout << ANSI::orange << "Destroyed Alt Scope From: " << func_token.args[2] << " \n" << ANSI::reset;
				result = *func_result;
				return;
			}
		}

	}
	emit_error(ERR_invalid_property_access, {get_variant_type_name(first.t), get_variant_type_name(second.t)});
}


const Operation OP_Access {
	OP_Access_exec,
};
