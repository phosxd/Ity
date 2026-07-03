#pragma once

#include <string>
#include <unordered_map>

#include "Common.hpp"
#include "ScriptErrors.hpp"
//#include "ExpressionParser.hpp"


// Create a new blank scope state.
ScopeState create_new_scope_state(std::unordered_map<std::string,Variant> data, ScopeState* parent = nullptr) {
	ScopeState result;
	if (parent != nullptr) {result.p = parent;}
	result.d = data;
	return result;
}


// Transforms "state" into a empty state & copies the current state into the new state's parent.
// This will effectively change the depth of the current scope while preserving the state reference.
void scope_in(ScopeState& state) {
	state.p = new ScopeState(create_new_scope_state(state.d, state.p));
	state.d = std::unordered_map<std::string,Variant>();
}


// Transforms "state" into a copy of it's parent state, deleting all data on the current state.
void scope_out(ScopeState& state) {
	ScopeState p;
	if (state.p != nullptr) {p = *state.p;}
	else {
		emit_error("Unexpected (ScopeState.scope_out): minimum depth reached, unable to scope out. Please report bug.");
		return;
	}
	state.p = p.p;
	state.d = p.d;
}


// Checks if the name is available in this scope.
bool is_name_free(ScopeState& state, std::string& name) {
	return (state.d.find(name) == state.d.end());
}


// Checks if the name is available in this scope & all scopes above it.
// Use this to check if a name could be shadowed.
bool is_name_globally_free(ScopeState& state, std::string& name) {
	ScopeState p;
	if (state.p != nullptr) {
		p = *state.p;
		return is_name_globally_free(p, name);
	}
	if (state.d.find(name) != state.d.end()) {return false;}
	return true;
}


// Gets the data for name in the current scope. Ensure the name exists in the current scope first.
Variant get_data(ScopeState& state, std::string& name) {
	if (state.d.find(name) == state.d.end()) {
		emit_error("Unexpected (ScopeState.get_data): cannot get non-existent data. Please report bug.");
		return Variant{};
	}
	return state.d[name];
}


// Gets the data for name in this scope or any scope above it. Ensure the name exists in one of the scopes (use is_name_globally_free).
Variant get_data_globally(ScopeState& state, std::string& name) {
	if (is_name_free(state, name) == false) {
		return get_data(state, name);
	}
	else if (state.p != nullptr) {
		return get_data_globally(*state.p, name);
	}
	else {
		emit_error("Unexpected (ScopeState.get_data_globally): cannot get non-existent data. Please report bug.");
		return Variant{};
	}
}


// Sets the data for name in this scope.
// If mode is "1" (constant), will throw an error when if the name is already taken in the current scope.
// If mode is "2" (locked type), will throw an error if the data type does not match the given type.
void set_data(ScopeState& state, std::string& name, std::string& type, VariantData& data, unsigned int mode) {
	if (is_name_free(state, name) == false) {
		Variant var = get_data(state, name);
		if (var.m == 1) {
			emit_error("Cannot change value of constant \"" + name + "\" after declaration.");
			return;
		}
		std::string data_type = get_variant_data_type(data);
		if (var.m == 2 && type != data_type) {
			emit_error("Cannot assign value of type \"" + data_type + "\" to a variable of type \"" + type + "\".");
			return;
		}
	}

	Variant var = Variant{};
	var.t = type;
	var.d = data;
	var.m = mode;
	state.d[name] = var;
}


void set_data_globally(ScopeState& state, std::string& name, std::string& type, VariantData& data, unsigned int mode) {
	if (is_name_free(state, name) == false) {
		set_data(state, name, type, data, mode);
	}
	else if (state.p != nullptr) {
		return set_data_globally(*state.p, name, type, data, mode);
	}
}


