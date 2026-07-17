#pragma once

#include <string>
#include <vector>



struct ScopeState;
struct InstToken;


namespace Ity {
	std::vector<InstToken> tokenize(const std::string& src);
	void exec(ScopeState& state, std::vector<InstToken>& sequence, const size_t start_idx, const int end_idx);
	void start_shell(int argc, char* argv[]);
}

