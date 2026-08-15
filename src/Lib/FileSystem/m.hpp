#pragma once




// Return whether or not a file exists or is readable.
Variant LIB_FileSystem_file_exists(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1) || not expect_arg_types(args[0], {STR}, 0)) return VariantPresets.none;
	const STR_t path = AnyCast(STR_t,args[0].d);
	return Variant{BOOL, std::filesystem::exists(path) && not std::filesystem::is_directory(path)};
}


// Return whether or not a file exists or is readable.
Variant LIB_FileSystem_dir_exists(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1) || not expect_arg_types(args[0], {STR}, 0)) return VariantPresets.none;
	const STR_t& path = AnyCast(STR_t,args[0].d);
	return Variant{BOOL, std::filesystem::exists(path) && std::filesystem::is_directory(path)};
}


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


// Write a string to a file. Returns `true` if succeeded, otherwise `false`.
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


// Creates a directory. Returns `true` if succeeded, otherwise `false`.
Variant LIB_FileSystem_make_dir(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1) || not expect_arg_types(args[0], {STR}, 0)) return VariantPresets.none;
	return Variant{BOOL, std::filesystem::create_directory(AnyCast(STR_t,args[0].d))};
}


// Removes a file or empty directory. Returns `true` if succeeded, otherwise `false`.
Variant LIB_FileSystem_remove(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1) || not expect_arg_types(args[0], {STR}, 0)) return VariantPresets.none;
	const STR_t& path = AnyCast(STR_t,args[0].d);
	return Variant{BOOL, ((not std::filesystem::is_directory(path) || std::filesystem::is_empty(path)) && std::filesystem::remove(path))};
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_FileSystem {
	MAP, (MAP_t){
		{"__name",  Variant{STR, (STR_t)"FileSystem", VariantMode_constant}},
		{"__safe",  Variant{BOOL, false}},

		{"file_exists",  NativeFuncTrans(BOOL,  (NativeFunc_t)LIB_FileSystem_file_exists)},
		{"dir_exists",   NativeFuncTrans(BOOL,  (NativeFunc_t)LIB_FileSystem_dir_exists)},
		{"read",         NativeFuncTrans(STR,   (NativeFunc_t)LIB_FileSystem_read)},
		{"write",        NativeFuncTrans(BOOL,  (NativeFunc_t)LIB_FileSystem_write)},
		{"make_dir",     NativeFuncTrans(BOOL,  (NativeFunc_t)LIB_FileSystem_make_dir)},
		{"remove",       NativeFuncTrans(BOOL,  (NativeFunc_t)LIB_FileSystem_remove)}
	},
	VariantMode_constant
};
