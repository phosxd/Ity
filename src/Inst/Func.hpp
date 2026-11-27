#pragma once


struct INST_Func_data {
	const std::string name = "";
	const VariantType type = VT_ANY;
	uint32_t sequence_size = 0;
};


static void INST_Func_processor(InstToken& token, const InstProcArgs& args) {
	VariantType type = VT_ANY;
	std::string name;

	if (token.args.size() == 2) name = token.args[1];
	else {
		type = get_variant_type_from_name(token.args[1]);
		name = token.args[2];
	}

	if (not is_valid_name(name)) {
		emit_error(ERR_name_must_not_contain_symbols, {name});
		return;
	}

	// Set token properties.
	token.data = new INST_Func_data{
		.name = std::move(name),
		.type = std::move(type)
	};
}


static void INST_Func_exec(ItyState& state, InstToken& token) {
	auto& data = *(INST_Func_data*)token.data;

	// Give error if the var name is not free on the current scope.
	if (state.scope.get_data(data.name)) {
		emit_error(ERR_name_is_taken, {data.name});
		return;
	}
	// Give warning if the var name is shadowing another var name.
	if (state.scope.get_data_globally(data.name)) {
		emit_warn(ERR_name_is_shadowed, {data.name});
	}

	// Create the function.
	state.scope.set_data(
		data.name, Variant{VT_FUNC, (FUNC_t){
			.return_type = data.type,
			.bound_args = (ARR_t){},
			.token_index = token.i,
			.definition_state_id = state.scope.id,
			.script_path = state.path
		}, VM_const}
	);
	// Jump past function body.
	state.exec_jump_value += token.composite_size;
}




const Instruction* INST_Func = new Instruction{
	.REQUIRED = 2,
	.exec = INST_Func_exec,
	.is_composite = true,
	.processor = INST_Func_processor
};
