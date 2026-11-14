#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#include "Registry.hpp"

#include <iostream>
#include <fstream>
#include <chrono>
#include <filesystem> // NOTE: "filesystem" is quirky across translation units, it is crucial it stays in the main file, otherwise we will get "std::bad_alloc" exceptions when trying to use it.


struct Variant;
struct ItyState;
struct ItyScope;
struct InstToken;


namespace Ity {
	using FLOAT64_t = double;
	using STR_t = std::string;
	using ARR_t = std::vector<Variant>;
	using MAP_t = std::unordered_map<STR_t,Variant>;

	inline const void* LIB_BI_G = nullptr; // Global reference to built-in lib.
	inline ARR_t* ARGS;


	void init();

	std::vector<InstToken> tokenize(const std::string& src);
	void exec(ItyState& state, const size_t start_idx, const int64_t end_idx);
	void start_shell(int argc, char* argv[]);
}

