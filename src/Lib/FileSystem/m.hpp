#pragma once




// Return whether or not a file exists or is readable.
static Variant LIB_FS_file_exists(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1) || not expect_arg_types(args[0], {STR}, 0)) return VPS.empty;
	const STR_t path = AnyCast(STR_t,args[0].d);
	return Variant{BOOL, std::filesystem::exists(path) && not std::filesystem::is_directory(path)};
}


// Return whether or not a file exists or is readable.
static Variant LIB_FS_dir_exists(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1) || not expect_arg_types(args[0], {STR}, 0)) return VPS.empty;
	const STR_t& path = AnyCast(STR_t,args[0].d);
	return Variant{BOOL, std::filesystem::exists(path) && std::filesystem::is_directory(path)};
}


// Read a file & return the bytes as a string. Returns `none` if failed.
static Variant LIB_FS_read(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1) || not expect_arg_types(args[0], {STR}, 0)) return VPS.empty;

	std::ifstream f (AnyCast(STR_t,args[0].d), std::ios::in | std::ios::binary);
	if (not f.is_open()) return VPS.empty;
	const std::string& bytes = (std::ostringstream() << f.rdbuf()).str();
	f.close();

	return Variant{STR, (STR_t)bytes};
}


// Write a string to a file. Returns `true` if succeeded, otherwise `false`.
static Variant LIB_FS_write(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 2) || not expect_arg_types(args[0], {STR}, 0) || not expect_arg_types(args[1], {STR}, 1)) return VPS.empty;
	const STR_t& bytes = AnyCast(STR_t,args[1].d);

	std::ofstream f (AnyCast(STR_t,args[0].d), std::ios::out | std::ios::binary);
	if (not f.is_open()) return VPS.bool_false;
	f.write(bytes.data(), bytes.size());
	f.close();

	return VPS.bool_true;
}


// Creates a directory. Returns `true` if succeeded, otherwise `false`.
static Variant LIB_FS_make_dir(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1) || not expect_arg_types(args[0], {STR}, 0)) return VPS.empty;
	return Variant{BOOL, std::filesystem::create_directory(AnyCast(STR_t,args[0].d))};
}


// Returns an array of strings representing all paths under the given directory.
static Variant LIB_FS_paths_in_dir(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1) || not expect_arg_types(args[0], {STR}, 0)) return VPS.empty;
	STR_t dir_path = AnyCast(STR_t,args[0].d);
	if (dir_path.empty()) dir_path = (STR_t)std::filesystem::current_path(); // Use current path if empty string given.
	ARR_t paths;

	// get all paths in the directory..
	if (std::filesystem::exists(dir_path) || dir_path.empty()) {
		for (const std::filesystem::path& path : std::filesystem::directory_iterator(dir_path)) {
			paths.push_back(Variant{
				STR,
				(STR_t)(path.filename()) + (std::filesystem::is_directory(path) ? "/" : ""), // Add a slash at the end to signify the path is a directory.
			});
		}
	}

	return Variant{ARR, paths};
}


// Removes a file or empty directory. Returns `true` if succeeded, otherwise `false`.
static Variant LIB_FS_remove(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 1) || not expect_arg_types(args[0], {STR}, 0)) return VPS.empty;
	const STR_t& path = AnyCast(STR_t,args[0].d);
	return Variant{BOOL, ((not std::filesystem::is_directory(path) || std::filesystem::is_empty(path)) && std::filesystem::remove(path))};
}


// Moves a file or directory. Returns `true` if succeeded, otherwise `false`.
static Variant LIB_FS_move(ItyState& _state, const ARR_t& args) {
	if (not expect_arg_count(args, 2) || not expect_arg_types(args[0], {STR}, 0) || not expect_arg_types(args[1], {STR}, 1)) return VPS.empty;
	const STR_t& path = AnyCast(STR_t,args[0].d);
	if (not std::filesystem::exists(path)) return VPS.bool_false;
	std::filesystem::rename(path, AnyCast(STR_t,args[1].d));
	return VPS.bool_true;
}




// DEFINE MAPPINGS
// ---------------

const Variant LIB_FileSystem {
	MAP, (MAP_t){
		{"__name",  Variant{STR, (STR_t)"FileSystem", VariantMode_constant}},
		{"__safe",  VPS.bool_false},

		{"file_exists",   NativeFuncTrans(BOOL,  (NativeFunc_t)LIB_FS_file_exists)},
		{"dir_exists",    NativeFuncTrans(BOOL,  (NativeFunc_t)LIB_FS_dir_exists)},

		{"read",          NativeFuncTrans(STR,   (NativeFunc_t)LIB_FS_read)},
		{"write",         NativeFuncTrans(BOOL,  (NativeFunc_t)LIB_FS_write)},

		{"make_dir",      NativeFuncTrans(BOOL,  (NativeFunc_t)LIB_FS_make_dir)},
		{"paths_in_dir",  NativeFuncTrans(ARR,   (NativeFunc_t)LIB_FS_paths_in_dir)},

		{"remove",        NativeFuncTrans(BOOL,  (NativeFunc_t)LIB_FS_remove)},
		{"move",          NativeFuncTrans(BOOL,  (NativeFunc_t)LIB_FS_move)},
	},
	VariantMode_constant
};
