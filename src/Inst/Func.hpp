#pragma once


void INST_Func_exec(const Instruction& inst, InstToken& token, ScopeState& state, const std::vector<std::string>& args) {
	const unsigned int args_len = args.size();
	const std::string& symbol = args[0];
	const std::string& type_name = args[1];
	const std::string& name = args[2];

	if (is_valid_name(name) == false) {
		emit_error(ERR_name_must_not_contain_symbols, {name});
		return;
	}

	// Give error if the var name is not free on the current scope.
	if (is_name_free(state, name) == false) {
		emit_error(ERR_name_is_taken, {name});
		return;
	}

	// Give warning if the var name is shadowing another var name.
	if (is_name_globally_free(state, name) == false) {
		emit_warn(ERR_name_is_shadowed, {name});
	}

	std::unordered_map<std::string,Variant> func_data = {
		{"__t", Variant(STR, (STR_t)"f")},
		{"__idx", Variant{INT, (int)token.i}},
		{"__ret_t", Variant{STR, type_name}}
	};
	set_data(state, name, MAP, func_data, VariantMode_constant);
	exec_jump_value += token.composite_size;
}


const Instruction INST_Func {
	3,               // Required arg count.
	0,               // Optional arg count.
	INST_Func_exec,  // Function.
	true,            // Is composite.
};
