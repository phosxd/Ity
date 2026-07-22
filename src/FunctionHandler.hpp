#pragma once


Variant call_script_function(ScopeState& state, const MAP_t& func, Variant& args) {
	// Throw error if maximum execution depth is reached.
	if (execution_depth > execution_depth_max) {
		emit_error(ERR_max_execution_depth, {std::to_string(execution_depth_max)});
		return VariantPresets.none;
	}

	func_arg_index = 0;
	const INT_t& func_token_index = AnyCast(INT_t,func.at("__i").d);
	const VariantType& func_return_type = AnyCast(VariantType,func.at("__rt").d );
	const INT_t& func_scope_depth = AnyCast(INT_t,func.at("__si").d);
	const InstToken& func_token = InstTokenSeq.at(func_token_index);

	// Throw error if token is not a function token.
	// The only time this should happen is if the tokens are corrupted in some way.
	if (func_token.args[0] != "func") {
		emit_error(ERR_unexpected, {"FunctionHandler", "Missing function: " + std::to_string(func_token_index) + " isn't a \"func\" token."});
		return VariantPresets.none;
	}

	const unsigned int func_body_start = func_token.i+1;
	const unsigned int& func_body_end = AnyCast(unsigned int, func_token.meta[0]);

	// Run function.
	call_trace.push_back(current_line); call_trace.push_back(current_column);
	push_back_ongoing_scopes();
	ScopeState func_state = create_new_scope_state(
		(MAP_t){
			{"__ARGS__", args},
			{"__RET__", Variant{func_return_type, std::any(), VariantMode_dynamic_type}},
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

	// Restore previous scope, then set result.
	call_trace.pop_back(); call_trace.pop_back();
	if (debug_flags.scoping) std::cout << ANSI::orange << "Destroyed Alt Scope From: " << func_token.args[2] << " \n" << ANSI::reset;

	return *func_result;
}
