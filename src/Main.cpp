#include <string>
#include <vector>
#include <unordered_map>

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

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


std::vector<InstToken> tokenize(std::string src) {
	const unsigned int src_len = src.size();
	std::vector<InstToken> sequence;
	std::string buffer;
	buffer.reserve(src_len);
	InstToken item;
	unsigned int ln = current_line;
	unsigned int col = current_column-1;

	bool is_start = true;
	bool is_comment = false;
	bool is_string = false;
	bool is_escaped_char = false;
	unsigned int str_start_ln = 1;
	unsigned int str_start_col = 0;
	char string_type;

	for (unsigned int i = 0; i < src_len; i++) {
		// Advance column or line number.
		col++;
		if(src[i] == '\n') {
			ln++;
			col = 0;
		}

		// End comment.
		if (is_comment) {
			if (src[i] == ';') {is_comment = false;}
			continue;
		}

		// Skip over spaces & tabs at the start of the item.
		if (is_start) {
			if (src[i] == ' ' || src[i] == '\n' || src[i] == '\t') {continue;}
			// Start comment.
			if (src[i] == '#') {
				is_comment = true;
				continue;
			}
			item = InstToken{ln,col};
			is_start = false;
		}

		if (is_string) {
			// End escaped.
			if (is_escaped_char == true) {
				is_escaped_char = false;
				buffer.push_back(src[i]);
				continue;
			}
			// Start escaped.
			if (src[i] == '\\') {
				is_escaped_char = true;
				buffer.push_back(src[i]); // Keep backslash, in case it is needed for an expression.
				continue;
			}
			// End string.
			if (src[i] == string_type) {
				is_string = false;
				buffer.push_back(src[i]);
				continue;
			}
		}

		else {
			// Start string.
			if (src[i] == '\'' || src[i] == '"') {
				is_string = true;
				string_type = src[i];
				str_start_ln = ln;
				str_start_col = col;
				buffer.push_back(src[i]);
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

		buffer.push_back(src[i]);
	}

	// Throw error if unterminated string.
	if (is_string) {
		emit_error("String literal has no end.", str_start_ln,str_start_col);
	}
	return sequence;
}




// Execute a sequence of instruction tokens.
ScopeState exec(std::vector<InstToken> sequence, ScopeState& state) {
	const unsigned int seq_len = sequence.size();

	for (unsigned int i = 0; i < seq_len; i++) {
		InstToken item = sequence[i];
		current_line = item.ln;
		current_column = item.col;
		const unsigned int arg_count = item.args.size();
		if (arg_count == 0) {continue;}

		// If not matched any instruction, run as expression.
		if (INSTRUCTIONS.find(item.args[0]) == INSTRUCTIONS.end()) {
			expr_exec(state, join_str(item.args, " "));
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
	// Get command line flags...
	std::vector<std::string> flags;
	for (unsigned int i = 0; i < argc; i++) {
		std::string arg_str (argv[i]);
		if (arg_str.size() < 2) {continue;}
		if (arg_str[0] == '-') {flags.push_back(arg_str);}
	}

	// Set debug flags
	if (exists_in_vec(flags, "-d-full")) {
		debug_flags.result = true;
		debug_flags.expr_seq = true;
		debug_flags.expr_result = true;
		debug_flags.data_assign = true;
	}
	if (exists_in_vec(flags, "-d-result")) {
		debug_flags.result = true;
	}
	if (exists_in_vec(flags, "-d-expr-seq")) {
		debug_flags.expr_seq = true;
	}
	if (exists_in_vec(flags, "-d-expr-result")) {
		debug_flags.expr_result = true;
	}
	if (exists_in_vec(flags, "-d-data-assign")) {
		debug_flags.data_assign = true;
	}


	ScopeState state = create_new_scope_state({});


	// Parse & execute script file...
	if (argc > 1 && argv[argc-1][0] != '-') {
		std::ifstream f (argv[argc-1], std::ios::in | std::ios::binary);
		if (f.is_open() == false) {
			std::cerr << "Unable to open script at \"" << argv[argc-1] << "\".\n";
			return 0;
		}

		// Read file.
		std::ostringstream ss; ss << f.rdbuf();
		std::string script = ss.str();
		f.close();

		// Start timer.
		clock_start = std::chrono::high_resolution_clock::now();

		// Tokenize the script.
		std::vector<InstToken> sequence = tokenize(script);
		// Execute tokens.
		exec(sequence, state);
	}


	// Run interactive interpreter...
	else {
		// Start timer.
		clock_start = std::chrono::high_resolution_clock::now();

		// Input loop...
		while (true) {
			std::cout << "\n>> ";
			std::string command;
			std::getline(std::cin, command);
			if (command == "exit") {
				break;
			}
			else {
				command += ';';
				// Tokenize the command.
				std::vector<InstToken> sequence = tokenize(command);
				// Execute tokens.
				exec(sequence, state);
			}
		}
	}


	// Output program results in debug mode.
	if (debug_flags.result) {
		auto end = std::chrono::high_resolution_clock::now();
		unsigned int micro = std::chrono::duration_cast<std::chrono::microseconds>(end-clock_start).count();
		unsigned int milli = std::chrono::duration_cast<std::chrono::milliseconds>(end-clock_start).count();
		std::cout << "\n\n" << "Program results...\n------------------\n";
		std::cout << "TIME: " << std::to_string(milli/1000.0) << "s (" << micro << "us).\n";
		std::cout << "STATE:\n	" << state << '\n';
		std::cout << '\n';
	}

	return 0;
}
