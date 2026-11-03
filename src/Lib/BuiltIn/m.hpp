#pragma once

#include <thread> // Needed for sleep.



// Override maximum execution depth.
static Variant LIB_BI_set_max_depth(ItyState& state, const ARR_t& args) {
	if (safe_mode) {
		emit_error(ERR_disallowed_member_in_safe_mode, {"set_max_depth"});
		return VPS.none;
	}

	const uint16_t& count = AnyCast(uint16_t,args[0].d);
	state.execution_depth_max = count;
	return VPS.none;
}


// Call a system comamnd. Returns the exit status code.
static Variant LIB_BI_system(ItyState& _state, const ARR_t& args) {
	if (safe_mode) {
		emit_error(ERR_disallowed_member_in_safe_mode, {"system"});
		return VPS.none;
	}

	const char* command = AnyCast(STR_t,args[0].d).c_str();
	return Variant{VT_UINT32, (uint32_t)system(command)};
}


// Pause thread execution for the given number of seconds.
static Variant LIB_BI_sleep(ItyState& _state, const ARR_t& args) {
	const Variant& var = args[0];
	std::this_thread::sleep_for(std::chrono::microseconds( (uint32_t)(((var.t == VT_UINT64) ? (FLOAT64_t)AnyCast(uint64_t,var.d) : AnyCast(FLOAT64_t,var.d))*1000000) ));
	return VPS.none;
}


// Return `true` if the name is defined in the current scope.
static Variant LIB_BI_is_defined(ItyState& state, const ARR_t& args) {
	return Variant{VT_BOOL, (state.scope.get_data_globally(AnyCast(STR_t,args[0].d)) != nullptr)};
}


// Return the type of the given Variant, in string form.
static Variant LIB_BI_type_name(ItyState& _state, const ARR_t& args) {
	return Variant{VT_STR, get_variant_type_name((VariantType)AnyCast(uint8_t,args[0].d))};
}


// Return the type of the given Variant.
static Variant LIB_BI_type(ItyState& _state, const ARR_t& args) {
	return Variant{VT_UINT8, (uint8_t)(args[0].t)};
}


// Return the number of bytes taken by the given variant.
static Variant LIB_BI_size(ItyState& state, ARR_t& args) {
	Variant* var = &args[0];
	return Variant{VT_INT64, (int64_t)(resovlve_potential_ref(state, var)->get_size())};
}


// Return an array of integers from the given start, end, & step.
static Variant LIB_BI_range(ItyState& _state, ARR_t& args) {
	int64_t step = 1;
	int64_t start = 0;
	int64_t end = AnyCastV(int64_t,args[0].d);
	if (args.size() > 1) {
		start = end;
		end = AnyCastV(int64_t,args[1].d);
	}
	if (args.size() > 2) {
		step = AnyCastV(int64_t,args[2].d);
	}


	ARR_t data; data.reserve(end);
	for (int64_t i = start; i < end; i += step) {
		data.push_back(Variant{VT_INT64, i});
	}

	return Variant{VT_ARR, data};
}




// Type methods.
// -------------


// Reference.
// ----------

// Reassign reference address.
static Variant LIB_BI_tm_ref_reassign(ItyState& _state, ARR_t& args) {
	// Get data.
	Variant* data = AnyCastV(Variant*,args[0].d);
	// Throw error if data is constant.
	if (data->m == VM_const) {
		emit_error(ERR_cannot_change_constant);
		return VPS.none;
	}
	// Reassign reference.
	data->d = AnyCastV(STR_t,args[1].d);
	return VPS.none;
}


// Get referenced data type.
static Variant LIB_BI_tm_ref_type(ItyState& state, const ARR_t& args) {
	// Get data.
	const Variant* data = AnyCastC(Variant*,args[0].d);
	// Return type.
	if (data->t == VT_PTR) return Variant{VT_UINT8, (uint8_t)AnyCastC(Variant*,data->d)->t};
	const Variant* var = state.scope.get_data_globally(AnyCast(STR_t,data->d));
	if (not var) return Variant{VT_UINT8, (uint8_t)VT_NONE};
	return Variant{VT_UINT8, (uint8_t)var->t};
}




// String.
// -------

// Returns the raw character code for the first character in the string.
static Variant LIB_BI_tm_str_raw(ItyState& _state, const ARR_t& args) {
	// Get data.
	const STR_t& data = AnyCast(STR_t, AnyCastC(Variant*,args[0].d)->d );
	// Return integer representation.
	if (data.empty()) return Variant{VT_INT64, (int64_t)-1};
	return Variant{VT_INT64, (int64_t)data[0]};
}


// String / Array.
// ---------------

// Return the length of the given array or string.
static Variant LIB_BI_tm_str_arr_length(ItyState& _state, const ARR_t& args) {
	const Variant* var = AnyCastC(Variant*,args[0].d);
	uint64_t size = 0;
	switch (var->t) {
		case VT_ARR: {size = AnyCastC(ARR_t,var->d).size(); break;}
		case VT_STR: {size = AnyCastC(STR_t,var->d).size(); break;}
		default: return VPS.none;
	}

	return Variant{VT_UINT64, size};
}


static Variant LIB_BI_tm_str_arr_append(ItyState& _state, ARR_t& args) {
	Variant* var = AnyCastV(Variant*,args[0].d);

	// Throw error if the variant is a constant.
	if (var->m == VM_const) {
		emit_error(ERR_cannot_change_constant);
		return VPS.none;
	}

	switch (var->t) {
		case VT_ARR: {
			ARR_t& data = AnyCastV(ARR_t,var->d);
			data.push_back(args[1]);
			break;
		}
		case VT_STR: {
			if (not expect_arg_types(args[1], {VT_STR, VT_INT64}, 0)) return VPS.none;
			STR_t& data = AnyCastV(STR_t,var->d);
			if (args[1].t == VT_STR) data.append(AnyCast(STR_t,args[1].d));
			else data.push_back(char(AnyCast(int64_t,args[1].d))); // Add character to the string, from a code.
			break;
		}
		default: break;
	}

	return VPS.none;
}


static Variant LIB_BI_tm_str_arr_reserve(ItyState& _state, ARR_t& args) {
	Variant* var = AnyCastV(Variant*,args[0].d);
	const int64_t& count = AnyCast(int64_t,args[1].d);

	// Throw error if the variant is a constant.
	if (var->m == VM_const) {
		emit_error(ERR_cannot_change_constant);
		return VPS.none;
	}

	switch (var->t) {
		case VT_ARR: {
			ARR_t& data = AnyCastV(ARR_t,var->d);
			data.reserve(count);
			break;
		}
		case VT_STR: {
			STR_t& data = AnyCastV(STR_t,var->d);
			data.reserve(count);
			break;
		}
		default: break;
	}

	return VPS.none;
}



// Array.
// ------

static Variant LIB_BI_tm_arr_map_erase(ItyState& _state, ARR_t& args) {
	Variant* var = AnyCastV(Variant*,args[0].d);

	// Throw error if the variant is a constant.
	if (var->m == VM_const) {
		emit_error(ERR_cannot_change_constant);
		return VPS.none;
	}

	if (var->t == VT_ARR) {
		if (not expect_arg_types(args[1], {VT_UINT64}, 1)) return VPS.none;
		// Get data & index.
		ARR_t& data = AnyCastV(ARR_t, var->d );
		const uint64_t& index = AnyCast(uint64_t,args[1].d);
		// Erase item from array reference.
		data.erase(data.begin()+index);
	}

	else if (var->t == VT_MAP) {
		if (not expect_arg_types(args[1], {VT_STR}, 1)) return VPS.none;
		// Get data & map.
		MAP_t& data = AnyCastV(MAP_t, var->d );
		const STR_t& key = AnyCast(STR_t,args[1].d);
		// Erase item from map reference.
		data.erase(key);
	}

	return VPS.none;
}




// Map.
// ----


// Return array of keys in the `MAP`.
static Variant LIB_BI_tm_map_keys(ItyState& _state, ARR_t& args) {
	// Get data.
	const MAP_t& data = AnyCast(MAP_t, AnyCastV(Variant*,args[0].d)->d );
	// Get all keys.
	ARR_t keys; keys.reserve(data.size());
	for (const auto& it : data) {
		keys.push_back(Variant{VT_STR, (STR_t)it.first});
	}
	// Return the keys.
	return Variant{VT_ARR, keys};
}


// Return whether or not the `MAP` has the given key.
static Variant LIB_BI_tm_map_has(ItyState& _state, ARR_t& args) {
	const MAP_t& data = AnyCast(MAP_t, AnyCastV(Variant*,args[0].d)->d );
	// Return whether or not the map has the key.
	if (data.find(AnyCast(STR_t,args[1].d)) != data.end()) return VPS.bool_true;
	return VPS.bool_false;
}


// Set key-value pair in the `MAP`.
static Variant LIB_BI_tm_map_set(ItyState& _state, ARR_t& args) {
	Variant* var = AnyCastV(Variant*,args[0].d);

	// Throw error if the variant is a constant.
	if (var->m == VM_const) {
		emit_error(ERR_cannot_change_constant);
		return VPS.none;
	}

	// Get data & key.
	MAP_t& data = AnyCastV(MAP_t, var->d );
	const STR_t& key = AnyCast(STR_t,args[1].d);

	// Set new key-value pair.
	data[key] = args[2];
	return VPS.none;
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
	return Variant{VT_FUNC, std::move(data)};
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_BI {
	VT_MAP, (MAP_t){
		{"__name",  Variant{VT_STR, (STR_t)"BI", VM_const}},
		{"__safe",  VPS.bool_true},


		// Type methods.
		{"__tm", Variant{
			VT_MAP, (MAP_t){
				{"REF:reassign",  NativeFuncTrans(VT_NONE,  LIB_BI_tm_ref_reassign, { {VT_PTR}, {VT_REF} })},
				{"REF:type",      NativeFuncTrans(VT_UINT8, (NativeFunc_t)LIB_BI_tm_ref_type,     { {VT_PTR} })},
				{"PTR:type",      NativeFuncTrans(VT_UINT8, (NativeFunc_t)LIB_BI_tm_ref_type,     { {VT_PTR} })},

				{"STR:raw",     NativeFuncTrans(VT_INT64,  (NativeFunc_t)LIB_BI_tm_str_raw,           { {VT_PTR} })},
				{"STR:length",  NativeFuncTrans(VT_UINT64, (NativeFunc_t)LIB_BI_tm_str_arr_length,    { {VT_PTR} })},
				{"STR:append",  NativeFuncTrans(VT_NONE,                 LIB_BI_tm_str_arr_append,    { {VT_PTR}, {VT_ANY} })},
				{"STR:reserve", NativeFuncTrans(VT_NONE,                 LIB_BI_tm_str_arr_reserve,   { {VT_PTR}, {VT_UINT64} })},
				{"ARR:length",  NativeFuncTrans(VT_UINT64, (NativeFunc_t)LIB_BI_tm_str_arr_length,    { {VT_PTR} })},
				{"ARR:erase",   NativeFuncTrans(VT_NONE,                 LIB_BI_tm_arr_map_erase,     { {VT_PTR}, {VT_UINT64,VT_STR} })},
				{"ARR:append",  NativeFuncTrans(VT_NONE,                 LIB_BI_tm_str_arr_append,    { {VT_PTR}, {VT_ANY} })},
				{"ARR:reserve", NativeFuncTrans(VT_NONE,                 LIB_BI_tm_str_arr_reserve,   { {VT_PTR}, {VT_UINT64} })},

				{"MAP:erase",   NativeFuncTrans(VT_NONE,  LIB_BI_tm_arr_map_erase, { {VT_PTR}, {VT_UINT64,VT_STR} })},
				{"MAP:keys",    NativeFuncTrans(VT_ARR,   LIB_BI_tm_map_keys,      { {VT_PTR} })},
				{"MAP:has",     NativeFuncTrans(VT_BOOL,  LIB_BI_tm_map_has,       { {VT_PTR}, {VT_STR} })},
				{"MAP:set",     NativeFuncTrans(VT_NONE,  LIB_BI_tm_map_set,       { {VT_PTR}, {VT_STR}, {VT_ANY} })},

				{"FUNC:bind",  NativeFuncTrans(VT_FUNC,  LIB_BI_tm_func_bind, { {VT_PTR}, {VT_ANY} })},
		}, VM_const_type }},


		// Type names.
		{"ANY",   var_type_var(VT_ANY)},
		{"PTR",   var_type_var(VT_PTR)},
		{"REF",   var_type_var(VT_REF)},
		{"NONE",  var_type_var(VT_NONE)},
		{"BOOL",  var_type_var(VT_BOOL)},

		{"INT",  var_type_var(VT_INT64)},
		{"I64",  var_type_var(VT_INT64)},
		{"I32",  var_type_var(VT_INT32)},
		{"I16",  var_type_var(VT_INT16)},
		{"I8",   var_type_var(VT_INT8)},

		{"UINT", var_type_var(VT_UINT64)},
		{"U64",  var_type_var(VT_UINT64)},
		{"U32",  var_type_var(VT_UINT32)},
		{"U16",  var_type_var(VT_UINT16)},
		{"U8",   var_type_var(VT_UINT8)},

		{"FLOAT",  var_type_var(VT_FLOAT64)},
		{"F64",    var_type_var(VT_FLOAT64)},

		{"STR",  var_type_var(VT_STR)},
		{"ARR",  var_type_var(VT_ARR)},
		{"MAP",  var_type_var(VT_MAP)},
		{"FUNC", var_type_var(VT_FUNC)},


		// Miscillanious constants.
		{"noneref", Variant{VT_NONE, std::monostate(), VM_const}},


		// Utility functions.
		{"set_max_depth",  NativeFuncTrans(VT_NONE,  (NativeFunc_t)LIB_BI_set_max_depth, { {VT_UINT16} })},

		{"system",      NativeFuncTrans(VT_UINT32, (NativeFunc_t)LIB_BI_system,     { {VT_STR} })},
		{"sleep",       NativeFuncTrans(VT_NONE,   (NativeFunc_t)LIB_BI_sleep,      { {VT_UINT64,VT_INT64,VT_FLOAT64} })},
		{"is_defined",  NativeFuncTrans(VT_BOOL,   (NativeFunc_t)LIB_BI_is_defined, { {VT_STR} })},
		{"type",        NativeFuncTrans(VT_UINT8,  (NativeFunc_t)LIB_BI_type,       { {VT_ANY} })},
		{"type_name",   NativeFuncTrans(VT_STR,    (NativeFunc_t)LIB_BI_type_name,  { {VT_UINT8} })},
		{"size",        NativeFuncTrans(VT_INT64,                LIB_BI_size,       { {VT_ANY} })},
		{"range",       NativeFuncTrans(VT_ARR,                  LIB_BI_range,      { {VT_INT64}, {VT_INT64, VT_NONE}, {VT_INT64, VT_NONE} })},

}, VM_const };
