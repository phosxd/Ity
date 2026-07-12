#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include "Common.hpp"
#include "ScriptErrors.hpp"


// Create a new blank scope state.
ScopeState create_new_scope_state(MAP_t data, ScopeState* parent = nullptr) {
	ScopeState result;
	if (parent != nullptr) {result.p = parent;}
	result.d = data;
	return result;
}


// Transforms "state" into a empty state & copies the current state into the new state's parent.
// This will effectively change the depth of the current scope while preserving the state reference.
void scope_in(ScopeState& state) {
	state.p = new ScopeState(create_new_scope_state(state.d, state.p));
	state.d = MAP_t();
}


// Transforms "state" into a copy of it's parent state, deleting all data on the current state.
void scope_out(ScopeState& state) {
	ScopeState p;
	if (state.p != nullptr) {p = *state.p;}
	else {
		emit_error(ERR_unexpected, {"ScopeState.scope_out", "minimum depth reached, unable to scope out."});
		return;
	}
	state.p = p.p;
	state.d = p.d;
}


// Returns the cumulative size of all Variants in the state data.
// This does *not* account for the data inside the state's parent.
unsigned int get_state_size(const ScopeState& state) {
	unsigned int final_size = 0;
	for (const auto& i : state.d) {
		final_size += sizeof(i.first) + sizeof(i.second);
	}
	return final_size;
}


// Checks if the name is available in this scope.
bool is_name_free(const ScopeState& state, const std::string& name) {
	return (state.d.find(name) == state.d.end());
}


// Checks if the name is available in this scope & all scopes above it.
// Use this to check if a name could be shadowed.
bool is_name_globally_free(const ScopeState& state, const std::string& name) {
	ScopeState p;
	if (state.p != nullptr) {
		return is_name_globally_free(*state.p, name);
	}
	if (state.d.find(name) != state.d.end()) {return false;}
	return true;
}


// Gets the data for name in the current scope. Ensure the name exists in the current scope first.
Variant get_data(ScopeState& state, const std::string& name) {
	if (state.d.find(name) == state.d.end()) {
		emit_error(ERR_unexpected, {"ScopeState.get_data", "cannot get non-existent data."});
		return VariantPresets.empty;
	}
	return state.d[name];
}


// Gets the data for name in this scope or any scope above it. Ensure the name exists in one of the scopes (use is_name_globally_free).
Variant get_data_globally(ScopeState& state, const std::string& name) {
	if (is_name_free(state, name) == false) {
		return get_data(state, name);
	}
	else if (state.p != nullptr) {
		return get_data_globally(*state.p, name);
	}
	else {
		emit_error(ERR_unexpected, {"ScopeState.get_data_globally", "cannot get non-existent data."});
		return VariantPresets.empty;
	}
}


// Sets the data for name in this scope.
// If mode is dynamic type, the set "type" & the actual type of "data" can be different.
// If mode is constant, will throw an error when if the name is already taken in the current scope.
// If mode is locked type, will throw an error if the data type does not match the given type.
void set_data(ScopeState& state, const std::string& name, const VariantType& type, const VariantData& data, const VariantMode& mode) {
	// Output function call in debug mode...
	if (debug_flags.data_assign) {
		std::cout << ANSI::blue << "Data assignment: " << ANSI::reset << "{name=" << name << ", type=" << type << ", data=" << data << ", mode=" << mode << "}\n";
	}

	if (not is_name_free(state, name)) {
		const Variant& var = get_data(state, name);
		if (var.m == 1) {
			emit_error(ERR_cannot_change_constant, {name});
			return;
		}
	}
	const VariantType& data_type = get_variant_data_type(data);
	if (mode != VariantMode_dynamic_type && type != data_type) {
		emit_error(ERR_assignment_type_mismatch, {get_variant_type_name(data_type), get_variant_type_name(type)});
		return;
	}

	state.d[name] = Variant {data_type, data, mode};
}


void set_data_globally(ScopeState& state, const std::string& name, const VariantType& type, const VariantData& data, const VariantMode mode) {
	if (not is_name_free(state, name)) {
		set_data(state, name, type, data, mode);
	}
	else if (state.p != nullptr) {
		return set_data_globally(*state.p, name, type, data, mode);
	}
}


