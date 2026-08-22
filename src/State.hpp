#pragma once

UINT_t ItyScope_current_id = 0;




#pragma pack(1)
struct ScopeItem {
	const size_t key;
	Variant var;
};


using ScopeMap_t = std::vector<ScopeItem>;

#pragma pack(1)
struct ItyScope {
	ItyScope* p = nullptr;  // Parent scope.
	ScopeMap_t d;           // Scope data.
	UINT_t id;              // Scope UID.


	// Returns the first parent scope with a matching ID.
	ItyScope* get_scope_at_id(unsigned int target_id) {
		if (id != target_id) {
			ItyScope* current = p;
			while (current) {
				if (current->id <= target_id) return current;
				current = current->p;
			}
		}

		return this;
	}


	// Returns the cumulative size of all Variants in the scope.
	// This does *not* account for the data inside the scope's parent.
	const unsigned int get_size() const {
		unsigned int final_size = 0;
		for (const ScopeItem& i : d) {
			final_size += sizeof(i.key) + i.var.get_size();
		}
		return final_size;
	}


	// Transforms into an empty scope & copies the current scope into the new scope's parent.
	// This will effectively change the depth of the current scope while preserving the scope reference.
	void in() {
		p = new ItyScope{std::move(p), std::move(d), std::move(id)};
		flush();
		ItyScope_current_id += 1;
		id = ItyScope_current_id;

		#ifdef RUNTIME_DEBUG
		if (debug_flags.scoping) std::cout << ANSI::orange << "Scope In (" + std::to_string(id) + ")\n" << ANSI::reset;
		#endif
	}


	// Transforms into a copy of it's parent scope, deleting all data on the current scope.
	void out() {
		if (not p) {
			emit_error(ERR_unexpected, {"ScopeOut", "Minimum depth reached."});
			return;
		}
		ItyScope* p_ = p;

		p = std::move(p_->p);
		d = std::move(p_->d);
		id =std::move(p_->id);
		delete p_;

		#ifdef RUNTIME_DEBUG
		if (debug_flags.scoping) std::cout << ANSI::orange << "Scope Out (" + std::to_string(id) + ")\n" << ANSI::reset;
		#endif
	}


	// Clears all data in the scope. Useful for loops that reuse the same scope.
	inline void flush() {
		d.clear();
	}


	// Get item container in the scope data from a hash value.
	ScopeItem* raw_get_data(const size_t& hashed_name, ScopeItem* default_value=nullptr) {
		for (ScopeItem& item : d) {
			if (item.key == hashed_name) return &item;
		}
		return default_value;
	}


	// Gets the data for name in the current scope. Returns `nullptr` or `default_value` if no data found.
	Variant* get_data(const std::string& name, Variant* default_value=nullptr, const size_t& hashed_name=0) {
		if (ScopeItem* item = raw_get_data((hashed_name == 0) ? string_hasher(name) : hashed_name); item) return &item->var;
		return default_value;
	}


	// Gets the data for name in this scope or any scope above it. Returns `nullptr` or `default_value` if no data found.
	Variant* get_data_globally(const std::string& name, Variant* default_value=nullptr, const size_t& hashed_name=0) {
		if (Variant* var = get_data(name, nullptr, hashed_name); var) return var;
		return (p) ? p->get_data_globally(name, default_value, hashed_name) : default_value;
	}


	inline void raw_set_data(const size_t& hashed_name, const Variant& data, ScopeItem*& item) {
		if (item) item->var = std::move(data);
		else d.push_back(ScopeItem(hashed_name, std::move(data)));
	}


	// Sets the data for name in this scope.
	void set_data(const std::string& name, const Variant& data, const size_t hashed_name) {
		// Output function call in debug mode...
		#ifdef RUNTIME_DEBUG
		if (debug_flags.data_assign) {
			std::cout << ANSI::blue << "Data Assignment: " << ANSI::reset << "{name=" << name << ", data=" << data << "}\n";
		}
		#endif

		ScopeItem* item = raw_get_data(hashed_name);
		if (item && item->var.m == VariantMode_constant) emit_error(ERR_cannot_change_constant); // Throw error if is a constant.
		raw_set_data(hashed_name, data, item);
	}


	void set_data_globally(const std::string& name, const Variant& data, const size_t& hashed_name=0) {
		return (p)
			? p->set_data_globally(name, data, hashed_name)
			: set_data(name, data, hashed_name)
		;
	}




	void merge_type_methods(MAP_t map) {
		Variant* var = get_data_globally("__tm__", nullptr, HASHED_NAMES.__tm__);
		if (var) AnyCastV(MAP_t,var->d).insert(map.begin(), map.end());
		else set_data("__tm__", Variant{MAP, map, VariantMode_locked_type}, HASHED_NAMES.__tm__);
	}


	// Merge all public members of the `map` into the scope.
	void merge_module(const MAP_t& map) {
		for (const auto& i : map) {
			const std::string& prop_name = i.first;

			// Check safety.
			if (prop_name == "__safe") {
				const bool& safe = AnyCast(bool,i.second.d);
				if (not safe && safe_mode) {
					emit_error(ERR_disallowed_member_in_safe_mode, {"(?)"});
					return;
				}
			}

			if (prop_name == "__tm") merge_type_methods(AnyCastV(MAP_t,i.second.d)); // Combine type methods.
			if (prop_name.starts_with("__")) continue; // Skip private members.

			// Throw error if name taken.
			if (get_data(prop_name)) {
				emit_error(ERR_name_is_taken, {prop_name});
				return;
			}
			// Copy member to a variable in the scope.
			set_data(prop_name, Variant{i.second.t, i.second.d, i.second.m}, string_hasher(prop_name));
		}
	}


	void import_module(const std::string& name, const MAP_t& map) {
		bool safe = true;
		const auto& safe_it = map.find("__safe");
		if (safe_it != map.end()) safe = AnyCast(bool,safe_it->second.d);
		if (not safe && safe_mode) {
			emit_error(ERR_disallowed_member_in_safe_mode, {"(?)"});
			return;
		}

		// Combine type methods.
		const auto& it = map.find("__tm");
		if (it != map.end()) merge_type_methods(AnyCast(MAP_t,it->second.d));

		// Copy module to a variable in the scope.
		set_data(name, Variant{MAP, map, VariantMode_constant}, string_hasher(name));
	}
};




// Create a new scope state.
ItyScope create_new_scope(const ScopeMap_t& data={}, ItyScope* parent=nullptr, UINT_t id=0) {
	if (id == 0) {
		ItyScope_current_id += 1;
		id = ItyScope_current_id;
	}

	return ItyScope{
		(parent) ? std::move(parent) : nullptr, // Moving `parent` is safe since it's already a copy of the pointer.
		std::move(data), // Yes, it is intentional that we move the referenced data, unsetting the original passed value.
		std::move(id),
	};
}




#pragma pack(1)
struct ItyState {
	std::string path = "";
	std::vector<ItyState> alts = {}; // Alternate states (script modules).
	std::vector<InstToken> seq = {}; // Instruction token sequence.
	ItyScope scope;

	Variant last_expr_result = VariantPresets.empty;
	// Instruction jump values.
	int exec_jump_value = 0;
	bool exec_jump_out = false;

	// This is a pool of all temporaries created in an expression.
	// Systems have exactly until the next expr exec call to use the data of a temporary, before it gets deleted.
	Variant temp_pool[MAX_TEMPORARY_POOL_RESERVE];
	uint8_t tp_c = 0;
	// Append new temporary variant. Throws error if over reserve size.
	inline Variant* append_temp_var(const Variant& var) {
		if (tp_c >= MAX_TEMPORARY_POOL_RESERVE) {
			emit_error(ERR_max_temporaries_in_use, {std::to_string(tp_c), std::to_string(MAX_TEMPORARY_POOL_RESERVE)});
			return &none_var;
		}
		temp_pool[tp_c] = std::move(var); tp_c++;
		return &temp_pool[tp_c-1];
	}

	// Tracker for tokens responsible for a scope.
	std::vector<InstToken*> scoped_tokens;


	void init() {
		// Add basic info.
		scope.d.push_back({string_hasher("__VERSION__"),              Variant{ARR,  (ARR_t){Variant{INT,ItyVersion[0]}, Variant{INT,ItyVersion[1]}, Variant{INT,ItyVersion[2]}, Variant{INT,ItyVersion[3]}}, VariantMode_constant}});
		scope.d.push_back({string_hasher("__VERSION_STRING__"),       Variant{STR,  (STR_t)ItyVersionString, VariantMode_constant}});
		scope.d.push_back({string_hasher("__SCRIPT_PATH__"),          Variant(STR,  (STR_t)path, VariantMode_constant)});
		scope.d.push_back({string_hasher("__OS_NAME__"),              Variant{STR,  (STR_t)OSName, VariantMode_constant}});
		scope.d.push_back({string_hasher("__HAS_RUNTIME_DEBUG__"),    Variant(BOOL, (bool)has_runtime_debug, VariantMode_constant)});
		scope.d.push_back({string_hasher("__CMD_ARGS__"),             Variant{ARR,  ARGS, VariantMode_constant}});
		// Merge built-in library.
		scope.merge_module(AnyCast(MAP_t,((Variant*)LIB_BI_G)->d));
		scope.in(); // Keep built-ins a scope higher than everything else.
	}


	ItyState* find_alt_from_path(std::string target_path) {
		for (ItyState& alt : alts) {
			if (alt.path == target_path) return &alt;
		}
		return nullptr;
	}


	// Scope out of all ongoing scopes & call emergency cleanup functions if needed.
	void exit_ongoing_scopes(ItyScope& scope) {
		for (InstToken* token : scoped_tokens) {
			if (token->inst->emergency_scope_exit) token->inst->emergency_scope_exit(token);
			scope.out();
		}
		scoped_tokens.clear();
	}
};
