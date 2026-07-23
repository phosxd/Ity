#pragma once


void INST_Import_exec(ScopeState& state, const Instruction* _inst, InstToken& _token, const std::vector<std::string>& args) {
	const std::string& symbol = args[0];
	std::string lib_name = args[1];
	std::string applied_name = lib_name;

	// Ok bro.
	if (lib_name.size() == 0) return;

	// Get lib name from variable.
	if (lib_name[0] == '@') {
		const std::string& var_name = lib_name.substr(1);
		// Throw error if variable name is bogus.
		if (is_name_globally_free(state, var_name)) {
			emit_error(ERR_name_does_not_exist, {var_name});
			return;
		}
		// Get variable.
		Variant var = *get_data_globally(state, var_name);
		// Convert to string if variable is not a string.
		if (var.t != STR) var.d = var_to_str(var);
		// Set lib name to look for as the variable value.
		lib_name = AnyCast(STR_t,var.d);
	}

	// Get alias.
	if (args.size() == 4) {
		if (args[2] != "as") {
			emit_error(ERR_invalid_syntax, {"Expected keyword \"as\""});
			return;
		}
		applied_name = args[3];
	}

	// Throw error if the name is already declared in this scope.
	if (not is_name_free(state, applied_name)) {
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

	// Throw error if library doesn't exist or is not allowed in safe mode.
	if (not lib || (safe_mode && not exists_in_vec(safe_mode_allowed_libs, lib_name))) {
		emit_error(ERR_unknown_module, {lib_name});
		return;
	}

	const MAP_t& lib_map = AnyCast(MAP_t,lib->d);
	//std::any_cast<NativeFunc_t>(lib_map.at("__init").d) (state, {}); // Call init function.
	// Merge all public members of the library into the scope.
	if (symbol == "merge") merge_module(state, lib_map);
	// Add library to scope with the given name.
	else import_module(state, applied_name, lib_map);
}


const Instruction INST_Import {
	2,                 // Required arg count.
	INST_Import_exec,  // Function.
	false,             // Is composite.
	false,             // Has expression.
};
