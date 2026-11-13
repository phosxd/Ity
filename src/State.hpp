#pragma once

#include <utility> // For "std::pair".


using ScopeMapCell_t = std::pair<uint32_t,Variant>;
using ScopeMap_t = std::vector<ScopeMapCell_t>;

struct ItyScope {
	ItyScope* p = nullptr;  // Parent scope.
	ScopeMap_t d;           // Scope data.
	uint32_t id;            // Scope UID.


	// Returns the first parent scope with a matching ID.
	ItyScope* get_scope_at_id(const uint32_t& target_id) {
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
	const size_t get_size() const {
		size_t final_size = 0;
		for (const std::pair<var_key_t,Variant>& pair : d) {
			final_size += pair.second.get_size();
		}
		return final_size;
	}


	// Transforms into an empty scope & copies the current scope into the new scope's parent.
	// This will effectively change the depth of the current scope while preserving the scope reference.
	void in(uint32_t& scope_current_id) {
		p = new ItyScope{std::move(p), std::move(d), std::move(id)};
		scope_current_id += 1;
		id = scope_current_id;

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
		*this = std::move(*p_);

		#ifdef RUNTIME_DEBUG
		if (debug_flags.scoping) std::cout << ANSI::orange << "Scope Out (" + std::to_string(id) + ")\n" << ANSI::reset;
		#endif
	}


	// Clears all data in the scope.
	void flush() {
		d.clear();
	}


	ScopeMapCell_t* get_data_cell(const var_key_t key=0) {
		for (ScopeMapCell_t& cell : d) {
			if (cell.first == key) return &cell;
		}
		return nullptr;
	}


	// Gets the data for name in the current scope. Returns `nullptr` or `default_value` if no data found.
	Variant* get_data(const std::string& name, Variant* default_value=nullptr, const var_key_t key=0) {
		ScopeMapCell_t* cell = get_data_cell((key == 0) ? string_hasher(name): key);
		return (cell) ? &cell->second : default_value;
	}


	// Gets the data for name in this scope or any scope above it. Returns `nullptr` or `default_value` if no data found.
	Variant* get_data_globally(const std::string& name, Variant* default_value=nullptr, const var_key_t key=0) {
		if (Variant* var = get_data(name, nullptr, key); var) return var;
		return (p) ? p->get_data_globally(name, default_value, key) : default_value;
	}


	// Get the Variant at `path`.
	// Path should use ":" to separate parts. Each part leads to the next Variant.
	Variant* resolve_path(const std::string& path, Variant* default_value=nullptr) {
		const std::vector<std::string> split_path = split_str(path, ':');
		Variant* result = nullptr;
		for (const std::string& part : split_path) {
			if (not result) result = get_data_globally(part);
			// Resolve array index...
			else if (result->t == VT_ARR) {
				ARR_t& data = AnyCastV(ARR_t,result->d);
				const size_t index = std::stoull(part);
				// Break if invalid index.
				if (index >= data.size()) {
					result = nullptr;
					break;
				}
				result = &data[index];
			}
			// Resolve map key...
			else if (result->t == VT_MAP) {
				MAP_t& data = AnyCastV(MAP_t,result->d);
				if (const MAP_t::iterator& it = data.find(part); it != data.end()) result = &it->second;
				// Break if invalid key.
				else {
					result = nullptr;
					break;
				}
			}
		}
		return (result) ? result : default_value;
	}


	// Sets the data for name in this scope.
	void set_data(const std::string& name, const Variant& data, var_key_t key=0) {
		if (key == 0) key = string_hasher(name.data());

		// Output function call in debug mode...
		#ifdef RUNTIME_DEBUG
		if (debug_flags.data_assign) {
			std::cout << ANSI::blue << "SetData: " << ANSI::reset << "{n=" << name << ", h=" << key << ", d=" << data << "}\n";
		}
		#endif

		ScopeMapCell_t* cell = get_data_cell(key);
		if (cell) {
			// Throw error if is a constant.
			if (cell->second.m == VM_const) {
				emit_error(ERR_cannot_change_constant);
				return;
			}
			cell->second = std::move(data);
		}
		else d.push_back( ScopeMapCell_t{key, std::move(data)} );
	}


	void set_data_globally(const std::string& name, const Variant& data, const var_key_t key=0) {
		return (p)
			? p->set_data_globally(name, data, key)
			: set_data(name, data, key)
		;
	}




	void merge_type_methods(const MAP_t& map) {
		Variant* var = get_data_globally("__tm__", nullptr, HASHED_NAMES.__tm__);
		if (var) AnyCastV(MAP_t,var->d).insert(map.begin(), map.end());
		else set_data("__tm__", Variant{VT_MAP, map, VM_const_type}, HASHED_NAMES.__tm__);
	}


	// Merge all public members of the `map` into the scope.
	void merge_module(const MAP_t& map) {
		for (const auto& i : map) {
			const std::string& prop_name = i.first;

			// Check safety.
			if (prop_name == "__safe") {
				const bool& safe = AnyCast(bool,i.second.d);
				// Emit error if in safe mode & module is unsafe.
				if (not safe && safe_mode) {
					emit_error(ERR_unsafe);
					return;
				}
			}

			if (prop_name == "__tm") merge_type_methods(AnyCast(MAP_t,i.second.d)); // Combine type methods.
			if (prop_name.starts_with("__")) continue; // Skip private members.

			// Throw error if name taken.
			if (get_data(prop_name)) {
				emit_error(ERR_name_is_taken, {prop_name});
				return;
			}
			// Copy member to a variable in the scope.
			set_data(prop_name, Variant{i.second.t, i.second.d, i.second.m});
		}
	}


	// Import a map as a module accessible by `name`.
	void import_module(const std::string& name, const MAP_t& map) {
		bool safe = true;
		const auto& safe_it = map.find("__safe");
		if (safe_it != map.end()) safe = AnyCast(bool,safe_it->second.d);
		// Emit error if in safe mode & module is unsafe.
		if (not safe && safe_mode) {
			emit_error(ERR_unsafe);
			return;
		}

		// Combine type methods.
		const auto& it = map.find("__tm");
		if (it != map.end()) merge_type_methods(AnyCast(MAP_t,it->second.d));

		// Copy module to a variable in the scope.
		set_data(name, Variant{VT_MAP, map, VM_const}, string_hasher(name));
	}
};




// Create a new scope.
ItyScope create_new_scope(uint32_t& scope_current_id, const ScopeMap_t& data={}, ItyScope* parent=nullptr, uint32_t id=0) {
	if (id == 0) {
		scope_current_id += 1;
		id = scope_current_id;
	}

	return ItyScope{
		(parent) ? std::move(parent) : nullptr, // Moving `parent` is safe since it's already a copy of the pointer.
		std::move(data), // Yes, it is intentional that we move the referenced data, unsetting the original passed value.
		std::move(id),
	};
}




struct ItyState {
	std::string path = "";
	std::vector<ItyState> alts = {}; // Alternate states (script modules).
	std::vector<InstToken> seq = {}; // Instruction token sequence.
	ItyScope scope;
	uint32_t scope_current_id = 0;
	std::vector<InstToken*> scoped_tokens; // Tracker for tokens responsible for a scope.

	uint16_t execution_depth_max = 1500;
	uint16_t execution_depth = 0;

	Variant last_expr_result = VPS.empty;
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


	void init() {
		// Add basic info.
		// Fragment Size: 1040
		scope.set_data("__VERSION__",              Variant{VT_ARR,  (ARR_t){Variant{VT_INT64,(int64_t)ItyVersion[0]}, Variant{VT_INT64,(int64_t)ItyVersion[1]}, Variant{VT_INT64,(int64_t)ItyVersion[2]}, Variant{VT_INT64,(int64_t)ItyVersion[3]}}, VM_const});
		scope.set_data("__VERSION_STRING__",       Variant{VT_STR,  (STR_t)ItyVersionString, VM_const});
		scope.set_data("__SCRIPT_PATH__",          Variant{VT_STR,  (STR_t)path, VM_const});
		scope.set_data("__OS_NAME__",              Variant{VT_STR,  (STR_t)OSName, VM_const});
		scope.set_data("__HAS_RUNTIME_DEBUG__",    Variant(VT_BOOL, has_runtime_debug, VM_const));
		scope.set_data("__CMD_ARGS__",             Variant{VT_ARR,  ARGS, VM_const});
		scope.set_data("__IMPORTED__",             VPS.bool_false);
		// Merge built-in library.
		scope.merge_module(AnyCast(MAP_t,((Variant*)LIB_BI_G)->d));
		scope.in(scope_current_id); // Keep built-ins a scope higher than everything else.
	}


	ItyState* find_alt_from_path(const std::string& target_path) {
		for (ItyState& alt : alts) {
			if (alt.path == target_path) return &alt;
		}
		return nullptr;
	}


	// Scope out of all ongoing scopes & call emergency cleanup functions if needed.
	void exit_ongoing_scopes(ItyScope& scope) {
		for (InstToken* token : scoped_tokens) {
			if (not token) continue;
			if (token->inst->emergency_scope_exit) token->inst->emergency_scope_exit(token);
			scope.out();
		}
		scoped_tokens.clear();
	}
};
