#pragma once


void INST_Func_exec(ScopeState& state, InstToken& token) {
	const std::string& type_name = token.args[1];
	const std::string& name = token.args[2];

	if (not is_valid_name(name)) {
		emit_error(ERR_name_must_not_contain_symbols, {name});
		return;
	}

	// Give error if the var name is not free on the current scope.
	if (get_data(state, name)) {
		emit_error(ERR_name_is_taken, {name});
		return;
	}

	// Give warning if the var name is shadowing another var name.
	if (get_data_globally(state, name)) {
		emit_warn(ERR_name_is_shadowed, {name});
	}

	set_data(
		state, name, FUNC,
		Variant{FUNC, (FUNC_t){
			get_variant_type_from_name(type_name), // Return type.
			(ARR_t){}, // Bound arguments.
			token.i,   // Token index.
			state.id,  // Scope state ID.
		}},
		VariantMode_constant
	);
	exec_jump_value += token.composite_size;
}




const Instruction* INST_Func = new Instruction{
	3,               // Required arg count.
	INST_Func_exec,  // Function.
	true,            // Is composite.
	false,           // Has expression.
};
