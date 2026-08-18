#pragma once


static void INST_Func_exec(ItyState& state, InstToken& token) {
	const std::string& type_name = token.args[1];
	const std::string& name = token.args[2];

	if (not is_valid_name(name)) {
		emit_error(ERR_name_must_not_contain_symbols, {name});
		return;
	}

	// Give error if the var name is not free on the current scope.
	if (state.scope.get_data(name)) {
		emit_error(ERR_name_is_taken, {name});
		return;
	}

	// Give warning if the var name is shadowing another var name.
	if (state.scope.get_data_globally(name)) {
		emit_warn(ERR_name_is_shadowed, {name});
	}

	// Create the function.
	state.scope.set_data(
		name, FUNC,
		Variant{FUNC, (FUNC_t){
			.return_type = get_variant_type_from_name(type_name),
			.bound_args = (ARR_t){},
			.token_index = token.i,
			.definition_state_id = state.scope.id,
			.script_path = state.path
		}},
		VariantMode_constant
	);
	// Jump past function body.
	state.exec_jump_value += token.composite_size;
}




const Instruction* INST_Func = new Instruction{
	.REQUIRED = 3,
	.exec = INST_Func_exec,
	.is_composite = true,
};
