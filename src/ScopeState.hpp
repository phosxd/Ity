#pragma once

#include "Common.hpp"
#include "ScriptErrors.hpp"

UINT_t ScopeState_current_id = 0;


ScopeState* get_state_at_id(ScopeState& state, unsigned int target_id) {
	if (state.id == target_id) return &state;
	ScopeState* current = state.p;
	while (true) {
		if (not current) break;
		if (current->id <= target_id) return current;
		current = current->p;
	}

	return &state;
}


// Create a new scope state.
ScopeState create_new_scope_state(const ScopeMap_t& data={}, ScopeState* parent=nullptr, UINT_t id=0) {
	if (id == 0) {
		ScopeState_current_id += 1;
		id = ScopeState_current_id;
	}

	return ScopeState{
		(parent) ? std::move(parent) : nullptr, // Moving `parent` is safe since it's already a copy of the pointer.
		std::move(data), // Yes, it is intentional that we move the referenced data, unsetting the original passed value.
		std::move(id),
	};
}


// Transforms "state" into a empty state & copies the current state into the new state's parent.
// This will effectively change the depth of the current scope while preserving the state reference.
void scope_in(ScopeState& state) {
	state.p = new ScopeState(create_new_scope_state(state.d, state.p, state.id));
	state.d.clear();
	ScopeState_current_id += 1;
	state.id = ScopeState_current_id;

	#ifdef RUNTIME_DEBUG
	if (debug_flags.scoping) std::cout << ANSI::orange << "Scope In (" + std::to_string(state.id) + ")\n" << ANSI::reset;
	#endif
}


// Transforms "state" into a copy of it's parent state, deleting all data on the current state.
void scope_out(ScopeState& state) {
	ScopeState* p = nullptr;
	if (state.p) p = state.p;
	else {
		emit_error(ERR_unexpected, {"ScopeOut", "Minimum depth reached."});
		return;
	}

	state.p = std::move(p->p);
	state.d = std::move(p->d);
	state.id =std::move(p->id);
	delete p;

	#ifdef RUNTIME_DEBUG
	if (debug_flags.scoping) std::cout << ANSI::orange << "Scope Out (" + std::to_string(state.id) + ")\n" << ANSI::reset;
	#endif
}


// Clears all data in the scope. Useful for loops that reuse the same scope.
inline void scope_flush(ScopeState& state) {
	state.d.clear();
}


// Scope out of all ongoing scopes & call emergency cleanup functions if needed.
void exit_ongoing_scopes(ScopeState& state) {
	for (InstToken* token : scoped_tokens) {
		if (token->inst->emergency_scope_exit) token->inst->emergency_scope_exit(token);
		scope_out(state);
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


// Returns the cumulative size of all Variants in the state data.
// This does *not* account for the data inside the state's parent.
const unsigned int get_state_size(const ScopeState& state) {
	unsigned int final_size = 0;
	for (const ScopeStateItem& i : state.d) {
		final_size += sizeof(i.key) + sizeof(i.var.t) + sizeof(i.var.m) + get_variant_size(i.var);
	}
	return final_size;
}


ScopeStateItem* raw_get_data(ScopeState& state, const size_t& hashed_name, ScopeStateItem* default_value=nullptr) {
	for (ScopeStateItem& item : state.d) {
		if (item.key == hashed_name) return &item;
	}
	return default_value;
}


// Gets the data for name in the current scope. Returns `nullptr` or `default_value` if no data found.
Variant* get_data(ScopeState& state, const std::string& name, Variant* default_value=nullptr) {
	if (ScopeStateItem* item = raw_get_data(state, string_hasher(name)); item) return &item->var;
	return default_value;
}


// Gets the data for name in this scope or any scope above it. Returns `nullptr` or `default_value` if no data found.
Variant* get_data_globally(ScopeState& state, const std::string& name, Variant* default_value=nullptr) {
	if (Variant* var = get_data(state, name); var) return var;
	else if (not state.p) return default_value;
	return get_data_globally(*state.p, name);
}


inline void raw_set_data(ScopeState& state, const size_t& hashed_name, const Variant& data) {
	if (ScopeStateItem* item = raw_get_data(state, hashed_name); item) item->var = data;
	else state.d.push_back(ScopeStateItem(hashed_name, data));
}


// Sets the data for name in this scope.
// If mode is dynamic type, the set "type" & the actual type of "data" can be different.
// If mode is constant, will throw an error when if the name is already taken in the current scope.
// If mode is locked type, will throw an error if the data type does not match the given type.
void set_data(ScopeState& state, const std::string& name, const VariantType& type, const Variant& data, const VariantMode& mode) {
	// Output function call in debug mode...
	#ifdef RUNTIME_DEBUG
	if (debug_flags.data_assign && not exists_in_vec(illegal_print_names, name)) {
		std::cout << ANSI::blue << "Data Assignment: " << ANSI::reset << "{name=" << name << ", type=" << type << ", data=" << data << ", mode=" << mode << "}\n";
	}
	#endif

	if (const Variant* var = get_data(state, name); var && var->m == VariantMode_constant) emit_error(ERR_cannot_change_constant); // Throw error if is a constant.
	if (mode != VariantMode_dynamic_type && type != data.t) emit_error(ERR_assignment_type_mismatch, {get_variant_type_name(data.t), get_variant_type_name(type)}); // Throw error if data is not applicable.
	raw_set_data(state, string_hasher(name), Variant{data.t, data.d, mode});
}


void set_data_globally(ScopeState& state, const std::string& name, const VariantType& type, const Variant& data, const VariantMode& mode) {
	if (get_data(state, name)) set_data(state, name, type, data, mode);
	else if (state.p) return set_data_globally(*state.p, name, type, data, mode);
}




// Merge all public members of the `map` into the scope `state`.
void merge_module(ScopeState& state, const MAP_t& map) {
	for (const auto& i : map) {
		const std::string& prop_name = i.first;

		if (prop_name == "__tm") {
			Variant* data = get_data(state, "__tm__");
			if (not data) raw_set_data(state, HASHED_NAMES.__tm__, i.second);
			else data->d = (AnyCast(MAP_t,data->d) + AnyCast(MAP_t,i.second.d));
		}

		if (prop_name.starts_with("__")) continue; // Skip private members.
		set_data(state, prop_name, i.second.t, i.second, i.second.m);
	}
}


void import_module(ScopeState& state, const std::string& name, const MAP_t& map) {
	const auto& it = map.find("__tm");
	if (it != map.end()) {
		Variant* data = get_data(state, "__tm__");
		if (not data) raw_set_data(state, HASHED_NAMES.__tm__, it->second);
		else data->d = (AnyCast(MAP_t,data->d) + AnyCast(MAP_t,it->second.d));
	}

	set_data(state, name, MAP, Variant{MAP, map}, VariantMode_constant);
}
