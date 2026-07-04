#include <string>
#include <vector>
#include <unordered_map>
#include <numeric>

#include <iostream>
#include <fstream>
#include <sstream>

#include "Common.hpp"
#include "ScriptErrors.hpp"
#include "ScopeState.hpp"
#include "ExpressionParser.hpp"

#include "Inst/Var.hpp"
#include "Inst/Set.hpp"


const std::unordered_map<std::string, Instruction> INSTRUCTIONS = {
	{"var", INST_Var},
	{"const", INST_Var},
	{"set", INST_Set},
};


InstToken new_inst_token(int ln, int col) {
	InstToken result;
	result.ln = ln;
	result.col = col;
	return result;
}


std::vector<InstToken> tokenize(std::string src) {
	const int src_len = src.size();
	std::vector<InstToken> sequence;
	std::string buffer;
	InstToken item;
	unsigned int ln = 1;
	unsigned int col = 0;

	bool is_start = true;
	bool is_comment = false;
	bool is_string = false;
	char string_type;

	for (unsigned int i = 0; i < src_len; i++) {
		// Advance column or line number.
		col++;
		if(src[i] == '\n') {
			ln++;
			col = 0;
		}

		// Skip over spaces & tabs at the start of the item.
		if (is_start) {
			if (src[i] == ' ' || src[i] == '\n' || src[i] == '\t') {continue;}
			item = new_inst_token(ln, col);
			is_start = false;
		}

		// End comment.
		if (is_comment) {
			if (src[i] == ';') {is_comment = false;}
			continue;
		}

		// End string.
		if (is_string) {
			if (src[i] == string_type) {
				is_string = false;
				buffer += src[i];
				continue;
			}
		}

		else {
			// Start comment.
			if (src[i] == '#' && item.args.size() == 0) {
				is_comment = true;
				continue;
			}

			// Start string.
			if (src[i] == '\'' || src[i] == '"') {
				is_string = true;
				string_type = src[i];
				buffer += src[i];
				continue;
			}

			// Add argument to args.
			if (src[i] == ' ' || src[i] == '\n') {
				if (buffer.size() > 0) {
					item.args.push_back(buffer);
					buffer = "";
				}
				continue;
			}

			// If is instruction end, append item to sequence & reset state.
			if (src[i] == ';') {
				// Append remaining argument to args.
				if (buffer.size() > 0) {
					item.args.push_back(buffer);
					buffer = "";
				}
				sequence.push_back(item);
				is_start = true;
				continue;
			}
		}

		buffer += src[i];
	}
	return sequence;
}




ScopeState exec(std::vector<InstToken> sequence) {
	const unsigned int seq_len = sequence.size();
	ScopeState state = create_new_scope_state({});
	for (unsigned int i = 0; i < seq_len; i++) {
		InstToken item = sequence[i];
		current_line = item.ln;
		current_column = item.col;
		const unsigned int arg_count = item.args.size();
		if (arg_count == 0) {continue;}

		// If not matched any instruction, run as expression.
		if (INSTRUCTIONS.find(item.args[0]) == INSTRUCTIONS.end()) {
			//expr_exec(state, std::reduce(item.args.begin(), item.args.end()));
		}
		// Execute instruction.
		else {
			Instruction inst = INSTRUCTIONS.at(item.args[0]);
			if (arg_count < inst.REQUIRED) {
				emit_error("Invalid number of arguments for instruction \"" + item.args[0] + "\". Expected at least " + std::to_string(inst.REQUIRED) + " separated by a space.");
				return state;
			}
			inst.exec(inst, state, item.args, item.args[0]);
		}
	}

	return state;
}




int main(int argc, char *argv[]) {
	if (argc == 2) {
		std::ifstream f (argv[1], std::ios::in | std::ios::binary);
		if (f.is_open() == false) {
			std::cerr << "Unable to open script at \"" << argv[1] << "\".\n";
			return 0;
		}

		// Read file.
		std::ostringstream ss; ss << f.rdbuf();
		std::string script = ss.str();

		// Tokenize the script.
		std::vector<InstToken> sequence = tokenize(script);
		// Execute tokens.
		ScopeState state = exec(sequence);
		// Output the state.
		std::cout << state << "\n";
	}

	return 0;
}
