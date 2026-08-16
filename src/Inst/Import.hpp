#pragma once


void INST_Import_exec(ItyState& state, InstToken& token) {
	std::string mod_name = token.args[1];
	if (mod_name.size() == 0) return;
	std::string applied_name = split_str(mod_name,'/').back();


	// Get lib name from variable.
	if (mod_name[0] == '@') {
		const std::string& var_name = mod_name.substr(1);
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
		mod_name = AnyCast(STR_t,var.d);
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


	const Variant* mod = nullptr;

	// Find module in built-in LIBS first.
	for (const Variant& item : LIBS) {
		if ( AnyCast(STR_t,AnyCast(MAP_t,item.d).at("__name").d) == mod_name) {
			mod = &item;
			break;
		}
	}

	// Find module from file..
	if (not mod) {
		std::string path = mod_name+ITY_FILE_EXT;
		std::ifstream f (path, std::ios::in | std::ios::binary);
		// If not in user defined path, check relative to global modules path.
		if (not f.is_open()) {
			path = GLOBAL_SHARE_PATH+"Modules/"+path;
			f = std::ifstream(path, std::ios::in | std::ios::binary);
		}

		// Run imported script & get defined module.
		if (f.is_open()) {
			const std::string& script = (std::ostringstream() << f.rdbuf()).str();
			f.close();
			current_script_path = path;
			const std::vector<InstToken> tokens = Ity::tokenize(script);
			state.alts.push_back(ItyState{
				.path = path,
				.seq = tokens,
				.scope = {.p = state.scope.get_scope_at_id(1)} // Use top-most scope as parent in the module scope.
			});
			ItyState& alt = state.alts.back();
			Ity::exec(alt, 0,-1);
			current_script_path = state.path; // Reset current script path.
			mod = alt.scope.get_data("__module__");
		}
	}

	// Throw error if module cannot be found.
	if (not mod || mod->t != MAP) {
		emit_error(ERR_unknown_module, {mod_name});
		return;
	}

	const MAP_t& mod_map = AnyCast(MAP_t,mod->d);
	// Merge all public members of the library into the scope.
	if (token.symbol == InstSymbol_merge) state.scope.merge_module(mod_map);
	// Add library to scope with the given name.
	else state.scope.import_module(applied_name, mod_map);
}




const Instruction* INST_Import = new Instruction{
	2,                 // Required arg count.
	INST_Import_exec,  // Function.
	false,             // Is composite.
	false,             // Has expression.
};
