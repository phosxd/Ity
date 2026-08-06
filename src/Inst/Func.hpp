#pragma once


void INST_Func_exec(ScopeState& state, InstToken& token) {
	const std::string& type_name = token.args[1];
	const std::string& name = token.args[2];

	if (not is_valid_name(name)) {
		emit_error(ERR_name_must_not_contain_symbols, {name});
		return;
	}

	// Give error if the var name is not free on the current scope.
	if (not is_name_free(state, name)) {
		emit_error(ERR_name_is_taken, {name});
		return;
	}

	// Give warning if the var name is shadowing another var name.
	if (not is_name_globally_free(state, name)) {
		emit_warn(ERR_name_is_shadowed, {name});
	}

	set_data(
		state, name, MAP,
		Variant{MAP, (MAP_t){
			{"__t",   VariantPresets.obj_type_f},         // Map type.
			{"__i",   Variant{INT, (INT_t)token.i}},      // Function token index.
			{"__si",  Variant{UINT, (UINT_t)(state.id)}}, // State ID.
			{"__rt",  Variant{INT, (INT_t)get_variant_type_from_name(type_name)}}, // Return type.
			{"__ba",  Variant{ARR, (ARR_t){}}}, // Bound args.
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
