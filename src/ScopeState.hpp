#pragma once

#include "Common.hpp"
#include "ScriptErrors.hpp"


unsigned int get_state_depth(const ScopeState& state) {
	unsigned int depth = 1;
	ScopeState* current = state.p;
	while (true) {
		if (current == nullptr) break;
		current = current->p;
		depth += 1;
	}
	return depth;
}


ScopeState* get_state_at_depth(ScopeState& state, const unsigned int target_depth) {
	unsigned int depth = get_state_depth(state);
	if (target_depth == depth) return &state;
	ScopeState* current = state.p;
	while (true) {
		if (not current) break;
		depth -= 1;
		if (depth == target_depth) return current;
		current = current->p;
	}

	emit_error(ERR_unexpected, {"ScopeState.get_state_at_depth", "No state at depth " + std::to_string(target_depth) + '.'});
	return &state;
}


// Create a new blank scope state.
ScopeState create_new_scope_state(const MAP_t& data, ScopeState* parent = nullptr) {
	ScopeState result;
	if (parent) result.p = parent;
	result.d = std::move(data);
	return result;
}


// Transforms "state" into a empty state & copies the current state into the new state's parent.
// This will effectively change the depth of the current scope while preserving the state reference.
void scope_in(ScopeState& state) {
	state.p = new ScopeState(create_new_scope_state(state.d, state.p));
	state.d.clear();
	if (debug_flags.scoping) std::cout << ANSI::orange << "Scope In (" + std::to_string(get_state_depth(state)) + ")\n" << ANSI::reset;
}


// Transforms "state" into a copy of it's parent state, deleting all data on the current state.
void scope_out(ScopeState& state) {
	ScopeState* p = nullptr;
	if (state.p) p = state.p;
	else {
		emit_error(ERR_unexpected, {"ScopeState.scope_out", "Minimum depth reached."});
		return;
	}
	state.p = std::move(p->p);
	state.d.clear();
	state.d = std::move(p->d);
	delete p;

	if (debug_flags.scoping) std::cout << ANSI::orange << "Scope Out (" + std::to_string(get_state_depth(state)) + ")\n" << ANSI::reset;
}


// Clears all data in the scope. Useful for loops that reuse the same scope.
inline void scope_flush(ScopeState& state) {
	state.d.clear();
}


// Scope out of all ongoing scopes & reset token metadata (if needed).
void exit_ongoing_scopes(ScopeState& state) {
	for (InstToken* token : scoped_tokens) {
		if (token->args.at(0) == "while") token->meta.clear();
		scope_out(state);
	}
	scoped_tokens.clear();
}


// Save ongoing scopes for later.
void push_back_ongoing_scopes() {
	scoped_tokens_stack.push_back(scoped_tokens);
	scoped_tokens = {};
}


// Restore last saved ongoing scopes.
void restore_ongoing_scopes() {
	scoped_tokens = scoped_tokens_stack.back();
	scoped_tokens_stack.pop_back();
}


// Returns the cumulative size of all Variants in the state data.
// This does *not* account for the data inside the state's parent.
unsigned int get_state_size(const ScopeState& state) {
	unsigned int final_size = 0;
	for (const auto& i : state.d) {
		final_size += i.first.size() + sizeof(i.second.t) + sizeof(i.second.m) + get_variant_data_size(i.second.d);
	}
	return final_size;
}


// Checks if the name is available in this scope.
bool is_name_free(const ScopeState& state, const std::string& name) {
	return state.d.find(name) == state.d.end();
}


// Checks if the name is available in this scope or any scopes above it.
bool is_name_globally_free(const ScopeState& state, const std::string& name) {
	if (state.d.find(name) == state.d.end()) {
		if (state.p) return is_name_globally_free(*state.p, name);
		return true;
	}
	return false;
}


// Gets the data for name in the current scope. Ensure the name exists in the current scope first.
Variant* get_data(ScopeState& state, const std::string& name) {
	const auto& it = state.d.find(name);
	if (it == state.d.end()) emit_error(ERR_unexpected, {"ScopeState.get_data", "Cannot get data."});
	return &it->second;
}


// Gets the data for name in this scope or any scope above it. Ensure the name exists in one of the scopes (use is_name_globally_free).
Variant* get_data_globally(ScopeState& state, const std::string& name) {
	if (not is_name_free(state, name)) return get_data(state, name);
	else if (not state.p) emit_error(ERR_unexpected, {"ScopeState.get_data_globally", "Cannot get data."});
	return get_data_globally(*state.p, name);
}


// Sets the data for name in this scope.
// If mode is dynamic type, the set "type" & the actual type of "data" can be different.
// If mode is constant, will throw an error when if the name is already taken in the current scope.
// If mode is locked type, will throw an error if the data type does not match the given type.
void set_data(ScopeState& state, const std::string& name, const VariantType& type, const VariantData& data, const VariantMode& mode) {
	// Output function call in debug mode...
	if (debug_flags.data_assign && not exists_in_vec(illegal_print_names, name)) {
		std::cout << ANSI::blue << "Data Assignment: " << ANSI::reset << "{name=" << name << ", type=" << type << ", data=" << data << ", mode=" << mode << "}\n";
	}

	const VariantType& data_type = get_variant_data_type(data);
	if (not is_name_free(state, name) && get_data(state, name)->m == VariantMode_constant) emit_error(ERR_cannot_change_constant); // Throw error if is a constant.
	if (mode != VariantMode_dynamic_type && type != data_type) emit_error(ERR_assignment_type_mismatch, {get_variant_type_name(data_type), get_variant_type_name(type)}); // Throw error if data is not applicable.
	state.d[name] = Variant{data_type, data, mode};
}


void set_data_globally(ScopeState& state, const std::string& name, const VariantType& type, const VariantData& data, const VariantMode& mode) {
	if (not is_name_free(state, name)) set_data(state, name, type, data, mode);
	else if (state.p) return set_data_globally(*state.p, name, type, data, mode);
}




// Merge all public members of the `map` into the scope `state`.
void merge_module(ScopeState& state, const MAP_t& map) {
	for (const auto& i : map) {
		const std::string& prop_name = i.first;

		if (prop_name == "__tm") {
			if (is_name_free(state, "__tm__")) state.d["__tm__"] = i.second;
			else state.d["__tm__"].d = (AnyCast(MAP_t,get_data(state, "__tm__")->d) + AnyCast(MAP_t,i.second.d));
		}

		if (prop_name.starts_with("__")) continue; // Skip private members.
		set_data(state, prop_name, i.second.t, i.second.d, i.second.m);
	}
}


void import_module(ScopeState& state, const std::string& name, const MAP_t& map) {
	const auto& it = map.find("__tm");
	if (it != map.end()) {
		if (is_name_free(state, "__tm__")) state.d["__tm__"] = it->second;
		else state.d["__tm__"].d = (AnyCast(MAP_t,get_data(state, "__tm__")->d) + AnyCast(MAP_t,it->second.d));
	}

	set_data(state, name, MAP, map, VariantMode_constant);
}
