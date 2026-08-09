#pragma once

#include <string>
#include <vector>



struct Variant;
struct ScopeState;
struct InstToken;


namespace Ity {
	void init();
	std::vector<InstToken> tokenize(const std::string& src);
	void exec(ScopeState& state, const size_t start_idx, const int end_idx);
	void start_shell(int argc, char* argv[]);
}

