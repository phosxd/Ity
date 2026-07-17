#pragma once


void INST_Import_exec(ScopeState& state, const Instruction* _inst, InstToken& _token, const std::vector<std::string>& args) {
	const std::string& symbol = args[0];
	const std::string& lib_name = args[1];
	std::string applied_name = lib_name;

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
		if ( std::any_cast<STR_t>(std::any_cast<MAP_t>(item.d).at("__name").d) == lib_name) {
			lib = &item;
			break;
		}
	}

	// Throw error if library doesn't exist.
	if (lib == nullptr) {
		emit_error(ERR_unknown_module, {lib_name});
		return;
	}

	const MAP_t& lib_map = std::any_cast<const MAP_t&>(lib->d);
	//std::any_cast<NativeFunc_t>(lib_map.at("__init").d) (state, {}); // Call init function.
	// Merge all public members of the library into the scope.
	if (symbol == "merge") merge_module(state, lib_map);
	// Add library to scope with the given name.
	else set_data(state, applied_name, MAP, lib_map, VariantMode_constant);
}


const Instruction INST_Import {
	2,                 // Required arg count.
	2,                 // Optional arg count.
	INST_Import_exec,  // Function.
	false,             // Is composite.
};
