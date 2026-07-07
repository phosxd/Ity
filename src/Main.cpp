#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <cstdint>

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

#include "ScriptErrors.hpp"
#include "Common.hpp"
#include "ScopeState.hpp"
#include "Expression.hpp"

#include "Inst/Var.hpp"
#include "Inst/Set.hpp"
#include "Inst/Jump.hpp"
#include "Inst/End.hpp"
#include "Inst/If.hpp"
#include "Inst/While.hpp"


const std::unordered_map<std::string, Instruction> INSTRUCTIONS = {
	{"var", INST_Var},
	{"const", INST_Var},
	{"set", INST_Set},
	{"jump", INST_Jump},
	{"/", INST_End},
	{"if", INST_If},
	{"elif", INST_If},
	{"else", INST_If},
	{"while", INST_While},
};


Variant last_expr_result;


struct CompositeItem {
	InstToken token;
	unsigned int index = 0;
	uint16_t size = 0;
	unsigned int ln = 0;
	unsigned int col = 0;
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
	unsigned int str_start_ln = ln;
	unsigned int str_start_col = col;
	char string_type;

	std::vector<CompositeItem> composite_nest;

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
				// Handle composite instructions.
				for (auto& comp_item : composite_nest) {
					// Throw error if composite size is about to go over the max for a 16-bit unsigned integer.
					if (comp_item.size == 65535) {
						emit_error("Exceeded maximum number of instructions under a composite instruction (65,535). This is not healthy. Please divorce your love for nesting.",ln,col);
						return sequence;
					}
					comp_item.size += 1;
				}
				if (item.args.size() > 0) {
					const std::string& inst_name = item.args[0];
					if (INSTRUCTIONS.find(inst_name) != INSTRUCTIONS.end()) {
						const Instruction& inst = INSTRUCTIONS.at(inst_name);
						// Start composite item...
						if (inst.is_composite) {
							composite_nest.push_back(CompositeItem {item, (unsigned int)sequence.size(), 0, ln, col});
						}
						// End composite item...
						else if (inst_name == "/") {
							if (composite_nest.size() > 0) {
								// Get & remove most recent composite item.
								CompositeItem comp_item = composite_nest.back();
								composite_nest.pop_back();
								// Apply updated token to sequence.
								comp_item.token.composite_size = comp_item.size;
								sequence[comp_item.index] = comp_item.token;
								item.linked_inst = comp_item.token.args[0];
								item.linked_inst_pos = -comp_item.size;
							}
							// Throw error if there is no composite to end.
							else {
								emit_error("There is no instruction requiring an \"end\" here.",ln,col);
								return sequence;
							}
						}
					}
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
		emit_error("String literal has no end.", str_start_ln, str_start_col);
	}
	// Throw error if unterminated composite.
	if (composite_nest.size() > 0) {
		const CompositeItem& comp_item = composite_nest[composite_nest.size()-1];
		emit_error("Composite instruction has no end.", comp_item.ln, comp_item.col);
	}
	if (debug_flags.inst_seq) {
		std::cout << ANSI::purple << "Instruction Sequence: " << ANSI::reset << sequence << '\n';
	}
	return sequence;
}




// Execute a sequence of instruction tokens.
ScopeState exec(std::vector<InstToken> sequence, ScopeState& state) {
	const unsigned int seq_len = sequence.size();
	unsigned int composite_nest = 0;
	for (unsigned int i = 0; i < seq_len; i++) {
		InstToken item = sequence[i];
		current_line = item.ln;
		current_column = item.col;
		const unsigned int arg_count = item.args.size();
		if (arg_count == 0) {continue;}

		// If not matched any instruction, run as expression.
		if (INSTRUCTIONS.find(item.args[0]) == INSTRUCTIONS.end()) {
			last_expr_result = expr_run(state, join_str(item.args, " "));
		}
		// Execute instruction.
		else {
			Instruction inst = INSTRUCTIONS.at(item.args[0]);
			if (arg_count < inst.REQUIRED) {
				emit_error("Invalid number of arguments for \"" + item.args[0] + "\". Expected at least " + std::to_string(inst.REQUIRED) + " separated by a space.");
				return state;
			}
			if (inst.exec != nullptr) {inst.exec(inst, item, state, item.args, item.args[0]);}
			if (exec_jump_value != 0) {
				i += exec_jump_value;
				exec_jump_value = 0;
			}
		}
	}
	return state;
}




int main(int argc, char *argv[]) {
	// Get command line flags...
	std::vector<std::string> script_args;
	std::vector<std::string> flags;
	std::string source_script_path = "";
	for (unsigned int i = 0; i < argc; i++) {
		if (i == 0) {continue;}
		std::string arg_str (argv[i]);
		if (arg_str.size() < 2) {continue;}
		if (arg_str[0] == '-') {flags.push_back(arg_str);}
		else if (source_script_path.empty()) {
			source_script_path = arg_str;
		}
		else {
			script_args.push_back(arg_str);
		}
	}

	// Set debug flags
	debug_flags.result = exists_in_vec(flags, "-d-result");
	debug_flags.inst_seq = exists_in_vec(flags, "-d-inst-seq");
	debug_flags.expr_seq = exists_in_vec(flags, "-d-expr-seq");
	debug_flags.expr_result = exists_in_vec(flags, "-d-expr-result");
	debug_flags.data_assign = exists_in_vec(flags, "-d-data-assign");
	if (exists_in_vec(flags, "-d-full")) {
		debug_flags.result = true;
		debug_flags.inst_seq = true;
		debug_flags.expr_seq = true;
		debug_flags.expr_result = true;
		debug_flags.data_assign = true;
	}


	ScopeState state = create_new_scope_state({
		{"CORE", Variant {
		}},
	});
	std::vector<ClockType> timers;
	for (unsigned int i = 0; i < 2; i++) {timers.push_back(Clock::now());}


	// Parse & execute script file...
	if (source_script_path.empty() == false) {
		if (str_ends_with(source_script_path,".ity") == false) {
			emit_error("Expected file with \".ity\" extension.");
			return 0;
		}
		std::ifstream f (source_script_path, std::ios::in | std::ios::binary);
		if (f.is_open() == false) {
			emit_error("Unable to open script at \"" + source_script_path + "\".");
			return 0;
		}

		// Read file.
		std::ostringstream ss; ss << f.rdbuf();
		std::string script = ss.str();
		f.close();

		current_line = 1;
		current_column = 1;
		clock_start = Clock::now();

		// Tokenize the script.
		timers[0] = Clock::now();
		std::vector<InstToken> sequence = tokenize(script);
		timers[1] = Clock::now();

		// Execute tokens.
		exec(sequence, state);
	}


	// Run interactive interpreter...
	else {
		std::cout << "* " << ANSI::yellow << "Ity (" << ItyVersionString << ")" << ANSI::reset << '\n'
			      << "* " << ANSI::purple << "Runing interactive mode interpreter. Run Ity code directly in the terminal!" << ANSI::reset << '\n'
			      << "* " << ANSI::purple << "Type \"exit\" to stop." << ANSI::reset << '\n';

		current_line = 1;
		current_column = 1;
		clock_start = std::chrono::high_resolution_clock::now();

		// Input loop...
		while (true) {
			std::cout << ANSI::purple << "\n>> " << ANSI::reset;
			std::string command;
			std::getline(std::cin, command);
			if (command == "exit") {
				break;
			}
			else {
				command += ';';
				last_expr_result = Variant{};

				// Tokenize the command.
				std::vector<InstToken> sequence = tokenize(command);
				// Execute tokens.
				exec(sequence, state);

				// Print expression result if there is one.
				if (last_expr_result.t != NONE) {
					std::cout << last_expr_result.d;
				}
			}
			current_line += 1;
			current_column = 1;
		}
	}


	// Output program results in debug mode.
	if (debug_flags.result) {
		auto total_end = std::chrono::high_resolution_clock::now();
		std::vector<std::vector<long int>> times = {
			{std::chrono::duration_cast<std::chrono::microseconds>(total_end-clock_start).count(),std::chrono::duration_cast<std::chrono::milliseconds>(total_end-clock_start).count()},
			{std::chrono::duration_cast<std::chrono::microseconds>(timers[1]-timers[0]).count(), std::chrono::duration_cast<std::chrono::milliseconds>(timers[1]-timers[0]).count()},
		};
		std::cout << "\n\n" << "Program results...\n------------------\n";
		if (source_script_path.empty() == false) {std::cout << "TIME (Inst-Tokenization): " << std::to_string(times[1][1]/1000.0) << "s (" << times[1][0] << "us).\n";}
		std::cout <<                                           "TIME (total):             " << std::to_string(times[0][1]/1000.0) << "s (" << times[0][0] << "us).\n";
		std::cout <<                                           "STATE:\n	" << state << '\n';
		std::cout << '\n';
	}

	return 0;
}
