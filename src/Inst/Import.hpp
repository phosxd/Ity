#pragma once


void INST_Import_exec(ItyState& state, InstToken& token) {
	std::string lib_name = token.args[1];
	std::string applied_name = lib_name;

	// Ok bro.
	if (lib_name.size() == 0) return;

	// Get lib name from variable.
	if (lib_name[0] == '@') {
		const std::string& var_name = lib_name.substr(1);
		Variant* var_ptr = state.scope.get_data_globally(var_name);
		// Throw error if var name is bogus.
		if (not var_ptr) {
			emit_error(ERR_name_does_not_exist, {var_name});
			return;
		}
		Variant var = *var_ptr;
		// Convert to string if var is not a string.
		if (var_ptr->t != STR) var.d = var_to_str(var);
		// Set lib name to look for as the variable value.
		lib_name = AnyCast(STR_t,var.d);
	}

	// Get alias.
	if (token.args.size() == 4) {
		if (token.args[2] != "as") {
			emit_error(ERR_invalid_syntax, {"Expected keyword \"as\""});
			return;
		}
		applied_name = token.args[3];
	}

	// Throw error if the name is already declared in this scope.
	if (state.scope.get_data(applied_name)) {
		emit_error(ERR_name_is_taken, {applied_name});
		return;
	}


	// Find library.
	const Variant* lib = nullptr;
	for (const Variant& item : LIBS) {
		if ( AnyCast(STR_t,AnyCast(MAP_t,item.d).at("__name").d) == lib_name) {
			lib = &item;
			break;
		}
	}

	// Throw error if library doesn't exist.
	if (not lib) {
		emit_error(ERR_unknown_module, {lib_name});
		return;
	}

	const MAP_t& lib_map = AnyCast(MAP_t,lib->d);
	//std::any_cast<NativeFunc_t>(lib_map.at("__init").d) (state, {}); // Call init function.
	// Merge all public members of the library into the scope.
	if (token.symbol == InstSymbol_merge) state.scope.merge_module(lib_map);
	// Add library to scope with the given name.
	else state.scope.import_module(applied_name, lib_map);
}




const Instruction* INST_Import = new Instruction{
	2,                 // Required arg count.
	INST_Import_exec,  // Function.
	false,             // Is composite.
	false,             // Has expression.
};
