#pragma once

UINT_t ItyScope_current_id = 0;




#pragma pack(1)
struct ScopeItem {
	const size_t key;
	Variant var;
};


using ScopeMap_t = std::vector<ScopeItem>;

// Modified by "While", "If", & "End" instructions.
std::vector<InstToken*> scoped_tokens;
// Managed by `ItyScope*_ongoing_scopes` functions.
std::vector<std::vector<InstToken*>> scoped_tokens_stack;


#pragma pack(1)
struct ItyScope {
	ItyScope* p = nullptr;  // Parent scope.
	ScopeMap_t d;           // Scope data.
	UINT_t id;              // Scope UID.


	// Returns the first parent scope with a matching ID.
	ItyScope* get_scope_at_id(unsigned int target_id) {
		if (id == target_id) return this;
		ItyScope* current = p;
		while (true) {
			if (not current) break;
			if (current->id <= target_id) return current;
			current = current->p;
		}

		return this;
	}


	// Transforms into an empty scope & copies the current scope into the new scope's parent.
	// This will effectively change the depth of the current scope while preserving the scope reference.
	void in() {
		p = new ItyScope{std::move(p), std::move(d), std::move(id)};
		d.clear();
		ItyScope_current_id += 1;
		id = ItyScope_current_id;

		#ifdef RUNTIME_DEBUG
		if (debug_flags.scoping) std::cout << ANSI::orange << "Scope In (" + std::to_string(id) + ")\n" << ANSI::reset;
		#endif
	}


	// Transforms into a copy of it's parent scope, deleting all data on the current scope.
	void out() {
		ItyScope* p_ = nullptr;
		if (p) p_ = p;
		else {
			emit_error(ERR_unexpected, {"ScopeOut", "Minimum depth reached."});
			return;
		}

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


	// Returns the cumulative size of all Variants in the scope.
	// This does *not* account for the data inside the scope's parent.
	const unsigned int get_size() {
		unsigned int final_size = 0;
		for (const ScopeItem& i : d) {
			final_size += sizeof(i.key) + get_variant_size(i.var);
		}
		return final_size;
	}


	// Get item container in the scope data from a hash value.
	ScopeItem* raw_get_data(const size_t& hashed_name, ScopeItem* default_value=nullptr) {
		for (ScopeItem& item : d) {
			if (item.key == hashed_name) return &item;
		}
		return default_value;
	}


	// Gets the data for name in the current scope. Returns `nullptr` or `default_value` if no data found.
	Variant* get_data(const std::string& name, Variant* default_value=nullptr, size_t hashed_name=0) {
		if (hashed_name == 0) hashed_name = string_hasher(name);
		if (ScopeItem* item = raw_get_data(hashed_name); item) return &item->var;
		return default_value;
	}


	// Gets the data for name in this scope or any scope above it. Returns `nullptr` or `default_value` if no data found.
	Variant* get_data_globally(const std::string& name, Variant* default_value=nullptr, size_t hashed_name=0) {
		if (Variant* var = get_data(name, nullptr, hashed_name); var) return var;
		else if (not p) return default_value;
		return p->get_data_globally(name, default_value, hashed_name);
	}


	inline void raw_set_data(const size_t& hashed_name, const Variant& data) {
		if (ScopeItem* item = raw_get_data(hashed_name); item) item->var = data;
		else d.push_back(ScopeItem(hashed_name, data));
	}


	// Sets the data for name in this scope.
	// If mode is dynamic type, the set "type" & the actual type of "data" can be different.
	// If mode is constant, will throw an error when if the name is already taken in the current scope.
	// If mode is locked type, will throw an error if the data type does not match the given type.
	void set_data(const std::string& name, const VariantType& type, const Variant& data, const VariantMode& mode, size_t hashed_name=0) {
		// Output function call in debug mode...
		#ifdef RUNTIME_DEBUG
		if (debug_flags.data_assign && not exists_in_vec(illegal_print_names, name)) {
			std::cout << ANSI::blue << "Data Assignment: " << ANSI::reset << "{name=" << name << ", type=" << type << ", data=" << data << ", mode=" << mode << "}\n";
		}
		#endif
		if (hashed_name == 0) hashed_name = string_hasher(name);

		if (const Variant* var = get_data(name, nullptr, hashed_name); var && var->m == VariantMode_constant) emit_error(ERR_cannot_change_constant); // Throw error if is a constant.
		if (mode != VariantMode_dynamic_type && type != data.t) emit_error(ERR_assignment_type_mismatch, {get_variant_type_name(data.t), get_variant_type_name(type)}); // Throw error if data is not applicable.

		raw_set_data(hashed_name, Variant{data.t, data.d, mode});
	}


	void set_data_globally(const std::string& name, const VariantType& type, const Variant& data, const VariantMode& mode, const size_t& hashed_name=0) {
		if (get_data(name)) set_data(name, type, data, mode, hashed_name);
		else if (p) return p->set_data_globally(name, type, data, mode, hashed_name);
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


// Scope out of all ongoing scopes & call emergency cleanup functions if needed.
void exit_ongoing_scopes(ItyScope& scope) {
	for (InstToken* token : scoped_tokens) {
		if (token->inst->emergency_scope_exit) token->inst->emergency_scope_exit(token);
		scope.out();
	}
	scoped_tokens.clear();
}


// Save ongoing scopes for later.
void push_back_ongoing_scopes() {
	scoped_tokens_stack.push_back(std::move(scoped_tokens));
	scoped_tokens = {};
}


// Restore last saved ongoing scopes.
void restore_ongoing_scopes() {
	scoped_tokens = scoped_tokens_stack.back();
	scoped_tokens_stack.pop_back();
}




// Merge all public members of the `map` into the scope.
void merge_module(ItyScope& scope, const MAP_t& map) {
	for (const auto& i : map) {
		const std::string& prop_name = i.first;

		if (prop_name == "__tm") {
			Variant* data = scope.get_data("__tm__");
			if (not data) scope.raw_set_data(HASHED_NAMES.__tm__, i.second);
			else data->d = (AnyCast(MAP_t,data->d) + AnyCast(MAP_t,i.second.d));
		}

		if (prop_name.starts_with("__")) continue; // Skip private members.
		scope.set_data(prop_name, i.second.t, i.second, i.second.m);
	}
}


void import_module(ItyScope& scope, const std::string& name, const MAP_t& map) {
	const auto& it = map.find("__tm");
	if (it != map.end()) {
		Variant* data = scope.get_data("__tm__");
		if (not data) scope.raw_set_data(HASHED_NAMES.__tm__, it->second);
		else data->d = (AnyCast(MAP_t,data->d) + AnyCast(MAP_t,it->second.d));
	}

	scope.set_data(name, MAP, Variant{MAP, map}, VariantMode_constant);
}




#pragma pack(1)
struct ItyState {
	std::vector<InstToken> seq = {}; // Instruction token sequence.
	ItyScope scope;
};


ItyState create_new_state(ItyScope scope) {
	return ItyState{
		.scope = scope,
	};
}
