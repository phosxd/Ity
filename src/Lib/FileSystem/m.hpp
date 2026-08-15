#pragma once


// Read a file & return the bytes as a string. Returns `none` if failed.
Variant LIB_FileSystem_read(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1) || not expect_arg_types(args[0], {STR}, 0)) return VariantPresets.none;
	const STR_t& path = AnyCast(STR_t,args[0].d);

	std::ifstream f (path, std::ios::in | std::ios::binary);
	if (not f.is_open()) return VariantPresets.none;
	const std::string& bytes = (std::ostringstream() << f.rdbuf()).str();
	f.close();

	return Variant{STR, (STR_t)bytes};
}


// Write a string to a file. Returns `true` operation succeeded, otherwise `false`.
Variant LIB_FileSystem_write(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 2) || not expect_arg_types(args[0], {STR}, 0) || not expect_arg_types(args[1], {STR}, 1)) return VariantPresets.none;
	const STR_t& path = AnyCast(STR_t,args[0].d);
	const STR_t& bytes = AnyCast(STR_t,args[1].d);

	std::ofstream f (path, std::ios::out | std::ios::binary);
	if (not f.is_open()) return VariantPresets.bool_false;
	f.write(bytes.data(), bytes.size());
	f.close();

	return VariantPresets.bool_true;
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_FileSystem {
	MAP, (MAP_t){
		{"__name",  Variant{STR, (STR_t)"FileSystem", VariantMode_constant}},
		{"__safe",  Variant{BOOL, false}},

		{"read",    NativeFuncTrans(STR,  (NativeFunc_t)LIB_FileSystem_read)},
		{"write",   NativeFuncTrans(BOOL, (NativeFunc_t)LIB_FileSystem_write)}
	},
	VariantMode_constant
};
