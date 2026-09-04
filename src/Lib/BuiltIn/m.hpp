#pragma once

#include <thread> // Needed for sleep.



// Override maximum execution depth.
static Variant LIB_BI_set_max_depth(ItyState* _state, const ARR_t& args) {
	if (safe_mode) {
		emit_error(ERR_disallowed_member_in_safe_mode, {"set_max_depth"});
		return VPS.empty;
	}
	if (not ExpectArgs(args, { {INT} })) return VPS.empty;

	const INT_t& count = AnyCast(INT_t,args[0].d);
	execution_depth_max = count;
	return VPS.empty;
}


// Call a system comamnd. Returns the exit status code.
static Variant LIB_BI_system(ItyState& _state, const ARR_t& args) {
	if (safe_mode) {
		emit_error(ERR_disallowed_member_in_safe_mode, {"system"});
		return VPS.empty;
	}
	if (not ExpectArgs(args, { {STR} })) return VPS.empty;

	const char* command = AnyCast(STR_t,args[0].d).c_str();
	return Variant{INT, (INT_t)system(command)};
}


// Pause thread execution for the given number of seconds.
static Variant LIB_BI_sleep(ItyState& _state, const ARR_t& args) {
	if (not ExpectArgs(args, { {INT,FLOAT} })) return VPS.empty;
	const Variant& var = args[0];
	std::this_thread::sleep_for(std::chrono::microseconds( (uint32_t)(((var.t == INT) ? AnyCast(INT_t,var.d) : AnyCast(FLOAT_t,var.d))*1000000) ));
	return VPS.empty;
}


// Return `true` if the name is defined in the current scope.
static Variant LIB_BI_is_defined(ItyState& state, const ARR_t& args) {
	if (not ExpectArgs(args, { {STR} })) return VPS.empty;
	return Variant{BOOL, (state.scope.get_data_globally(AnyCast(STR_t,args[0].d)) != nullptr)};
}


// Return the type of the given Variant, in string form.
static Variant LIB_BI_type_name(ItyState& _state, const ARR_t& args) {
	if (not ExpectArgs(args, { {INT} })) return VPS.empty;
	return Variant{STR, get_variant_type_name((VariantType)AnyCast(INT_t,args[0].d))};
}


// Return the type of the given Variant.
static Variant LIB_BI_type(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VPS.empty;
	return Variant{INT, (INT_t)(args[0].t)};
}


// Return the number of bytes taken by the given variant.
static Variant LIB_BI_size(ItyState& state, const ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VPS.empty;
	const Variant* var = &args[0];
	return Variant{INT, (INT_t)(resovlve_potential_ref(state, var)->get_size())};
}


// Return an array of integers from the given start, end, & step.
static Variant LIB_BI_range(ItyState& _state, const ARR_t& args) {
	if (args.size() == 0) {
		emit_error(ERR_invalid_func_arg_count, {"1+", "0"});
		return VPS.empty;
	}
	if (not expect_arg_types(args[0], {INT}, 0)) return VPS.empty;

	INT_t step = 1;
	INT_t start = 0;
	INT_t end = AnyCastV(INT_t,args[0].d);
	if (args.size() > 1) {
		if (not expect_arg_types(args[1], {INT}, 1)) return VPS.empty;
		start = end;
		end = AnyCastV(INT_t,args[1].d);
	}
	if (args.size() > 2) {
		if (not expect_arg_types(args[2], {INT}, 2)) return VPS.empty;
		step = AnyCastV(INT_t,args[2].d);
	}


	ARR_t data; data.reserve(end);
	for (INT_t i = start; i < end; i += step) {
		data.push_back(Variant{INT, i});
	}

	return Variant{ARR, data};
}




// Type methods.
// -------------


// Reference.
// ----------

// Reassign reference address.
static Variant LIB_BI_tm_ref_reassign(ItyState& _state, const ARR_t& args) {
	if (not ExpectArgs(args, { {PTR}, {REF} })) return VPS.empty;
	// Get data.
	Variant* data = AnyCastV(Variant*,args[0].d);
	// Throw error if data is constant.
	if (data->m == VariantMode_constant) {
		emit_error(ERR_cannot_change_constant);
		return VPS.empty;
	}
	// Reassign reference.
	data->d = AnyCastV(STR_t,args[1].d);
	return VPS.empty;
}


// Get referenced data type.
static Variant LIB_BI_tm_ref_type(ItyState& state, const ARR_t& args) {
	if (not ExpectArgs(args, { {PTR} })) return VPS.empty;
	// Get data.
	Variant* data = AnyCastV(Variant*,args[0].d);
	// Return type.
	if (data->t == PTR) return Variant{INT, (INT_t)AnyCast(Variant*,data->d)->t};
	const Variant* var = state.scope.get_data_globally(AnyCast(STR_t,data->d));
	if (not var) return Variant{INT, (INT_t)NONE};
	return Variant{INT, (INT_t)var->t};
}




// String.
// -------

// Returns the raw character code for the first character in the string.
static Variant LIB_BI_tm_str_raw(ItyState& _state, ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VPS.empty;
	// Get data.
	const STR_t& data = AnyCast(STR_t, AnyCastV(Variant*,args[0].d)->d );
	// Return integer representation.
	if (data.empty()) return Variant{INT, (INT_t)-1};
	return Variant{INT, (INT_t)data[0]};
}


// String / Array.
// ---------------

// Return the length of the given array or string.
static Variant LIB_BI_tm_str_arr_length(ItyState& _state, const ARR_t& args) {
	Variant* var = AnyCastV(Variant*,args[0].d);
	INT_t size = 0;
	switch (var->t) {
		case ARR: {size = AnyCast(ARR_t,var->d).size(); break;}
		case STR: {size = AnyCast(STR_t,var->d).size(); break;}
		default: return VPS.empty;
	}

	return Variant{INT, size};
}



// Array.
// ------

static Variant LIB_BI_tm_arr_map_erase(ItyState& _state, ARR_t& args) {
	if (not expect_arg_count(args, 2)) return VPS.empty;

	Variant* var = AnyCastV(Variant*,args[0].d);

	// Throw error if the variant is a constant.
	if (var->m == VariantMode_constant) {
		emit_error(ERR_cannot_change_constant);
		return VPS.empty;
	}

	if (var->t == ARR) {
		if (not expect_arg_types(args[1], {INT}, 1)) return VPS.empty;
		// Get data & index.
		ARR_t& data = AnyCastV(ARR_t, var->d );
		const INT_t& index = AnyCast(INT_t,args[1].d);
		// Erase item from array reference.
		data.erase(data.begin()+index);
	}

	else if (var->t == MAP) {
		if (not expect_arg_types(args[1], {STR}, 1)) return VPS.empty;
		// Get data & map.
		MAP_t& data = AnyCastV(MAP_t, var->d );
		const STR_t& key = AnyCast(STR_t,args[1].d);
		// Erase item from map reference.
		data.erase(key);
	}

	return VPS.empty;
}


static Variant LIB_BI_tm_arr_append(ItyState& _state, ARR_t& args) {
	if (not expect_arg_count(args, 2)) return VPS.empty;
	Variant* var = AnyCast(Variant*,args[0].d);

	// Throw error if the variant is a constant.
	if (var->m == VariantMode_constant) {
		emit_error(ERR_cannot_change_constant);
		return VPS.empty;
	}

	ARR_t& data = AnyCastV(ARR_t,var->d);
	data.push_back(args[1]);
	return VPS.empty;
}


static Variant LIB_BI_tm_arr_reserve(ItyState& _state, ARR_t& args) {
	if (not expect_arg_count(args, 2) || not expect_arg_types(args[1], {INT}, 1)) return VPS.empty;
	Variant* var = AnyCast(Variant*,args[0].d);
	const INT_t& count = AnyCast(INT_t,args[1].d);

	// Throw error if the variant is a constant.
	if (var->m == VariantMode_constant) {
		emit_error(ERR_cannot_change_constant);
		return VPS.empty;
	}

	ARR_t& data = AnyCastV(ARR_t,var->d);
	data.reserve(count);
	return VPS.empty;
}




// Map.
// ----


// Return array of keys in the `MAP`.
static Variant LIB_BI_tm_map_keys(ItyState& _state, ARR_t& args) {
	if (not expect_arg_count(args, 1)) return VPS.empty;

	// Get data.
	const MAP_t& data = AnyCast(MAP_t, AnyCastV(Variant*,args[0].d)->d );
	// Get all keys.
	ARR_t keys; keys.reserve(data.size());
	for (const auto& it : data) {
		keys.push_back(Variant{STR, (STR_t)it.first});
	}
	// Return the keys.
	return Variant{ARR, keys};
}


// Return whether or not the `MAP` has the given key.
static Variant LIB_BI_tm_map_has(ItyState& _state, ARR_t& args) {
	if (not expect_arg_count(args, 2) || not expect_arg_types(args[1], {STR}, 1)) return VPS.empty;
	const MAP_t& data = AnyCast(MAP_t, AnyCastV(Variant*,args[0].d)->d );
	// Return whether or not the map has the key.
	if (data.find(AnyCast(STR_t,args[1].d)) != data.end()) return VPS.bool_true;
	return VPS.bool_false;
}


// Set key-value pair in the `MAP`.
static Variant LIB_BI_tm_map_set(ItyState& _state, ARR_t& args) {
	if (not expect_arg_count(args, 3) || not expect_arg_types(args[1], {STR}, 1)) return VPS.empty;

	Variant* var = AnyCastV(Variant*,args[0].d);

	// Throw error if the variant is a constant.
	if (var->m == VariantMode_constant) {
		emit_error(ERR_cannot_change_constant);
		return VPS.empty;
	}

	// Get data & key.
	MAP_t& data = AnyCast(MAP_t, var->d );
	const STR_t& key = AnyCast(STR_t,args[1].d);

	// Set new key-value pair.
	data[key] = args[2];
	return VPS.empty;
}




// Function.
// ---------

// Bind arguments to the function.
static Variant LIB_BI_tm_func_bind(ItyState& _state, ARR_t& args) {
	FUNC_t data = AnyCastV(FUNC_t, AnyCastV(Variant*,args[0].d)->d ); // Copy the function.
	// Add the given args to the function's bounded arguments.
	for (size_t i = 1; i < args.size(); i++) {
		data.bound_args.push_back(args[i]);
	}
	// Return the new function.
	return Variant{FUNC, std::move(data)};
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_BI {
	MAP, (MAP_t){
		{"__name",  Variant{STR, (STR_t)"BI", VariantMode_constant}},
		{"__safe",  VPS.bool_true},


		// Type methods.
		{"__tm", Variant{
			MAP, (MAP_t){
				{"REF:reassign",  NativeFuncTrans(NONE, (NativeFunc_t)LIB_BI_tm_ref_reassign)},
				{"REF:type",      NativeFuncTrans(INT,  (NativeFunc_t)LIB_BI_tm_ref_type)},
				{"PTR:type",      NativeFuncTrans(INT,  (NativeFunc_t)LIB_BI_tm_ref_type)},

				{"STR:raw",     NativeFuncTrans(INT,   (NativeFunc_t)LIB_BI_tm_str_raw)},
				{"STR:length",  NativeFuncTrans(INT,   (NativeFunc_t)LIB_BI_tm_str_arr_length)},
				{"ARR:length",  NativeFuncTrans(INT,   (NativeFunc_t)LIB_BI_tm_str_arr_length)},
				{"ARR:erase",   NativeFuncTrans(NONE,  (NativeFunc_t)LIB_BI_tm_arr_map_erase)},
				{"ARR:append",  NativeFuncTrans(NONE,  (NativeFunc_t)LIB_BI_tm_arr_append)},
				{"ARR:reserve", NativeFuncTrans(NONE,  (NativeFunc_t)LIB_BI_tm_arr_reserve)},

				{"MAP:erase",   NativeFuncTrans(NONE,  (NativeFunc_t)LIB_BI_tm_arr_map_erase)},
				{"MAP:keys",    NativeFuncTrans(ARR,   (NativeFunc_t)LIB_BI_tm_map_keys)},
				{"MAP:has",     NativeFuncTrans(BOOL,  (NativeFunc_t)LIB_BI_tm_map_has)},
				{"MAP:set",     NativeFuncTrans(NONE,  (NativeFunc_t)LIB_BI_tm_map_set)},

				{"FUNC:bind",  NativeFuncTrans(MAP,  (NativeFunc_t)LIB_BI_tm_func_bind)},
		}, VariantMode_locked_type }},


		// Type names.
		{"ANY",    var_type_var(ANY)},
		{"PTR",    var_type_var(PTR)},
		{"REF",    var_type_var(REF)},
		{"NONE",   var_type_var(NONE)},
		{"BOOL",   var_type_var(BOOL)},
		{"INT",    var_type_var(INT)},
		{"FLOAT",  var_type_var(FLOAT)},
		{"STR",    var_type_var(STR)},
		{"ARR",    var_type_var(ARR)},
		{"MAP",    var_type_var(MAP)},
		{"FUNC",   var_type_var(FUNC)},


		// Miscillanious constants.
		{"noneref", Variant{NONE, std::monostate(), VariantMode_constant}},


		// Utility functions.
		{"set_max_depth",  NativeFuncTrans(NONE,  (NativeFunc_t)LIB_BI_set_max_depth)},

		{"system",      NativeFuncTrans(INT,   (NativeFunc_t)LIB_BI_system)},
		{"sleep",       NativeFuncTrans(NONE,  (NativeFunc_t)LIB_BI_sleep)},
		{"is_defined",  NativeFuncTrans(BOOL,  (NativeFunc_t)LIB_BI_is_defined)},
		{"type",        NativeFuncTrans(INT,   (NativeFunc_t)LIB_BI_type)},
		{"type_name",   NativeFuncTrans(STR,   (NativeFunc_t)LIB_BI_type_name)},
		{"size",        NativeFuncTrans(INT,   (NativeFunc_t)LIB_BI_size)},
		{"range",       NativeFuncTrans(ARR,   (NativeFunc_t)LIB_BI_range)},

}, VariantMode_constant };
