#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <any>

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

#include "Util.hpp"
#include "ScriptErrors.hpp"
#include "Common.hpp"
#include "ScopeState.hpp"
#include "Expression.hpp"

// Lib imports...
#include "Lib/BuiltIn.hpp"
#include "Lib/IO.hpp"
#include "Lib/Time.hpp"
#include "Lib/Math.hpp"
const Variant LIBS[] = {
	LIB_BI,
	LIB_IO,
	LIB_Time,
	LIB_Math
};

// Instruction imports...
#include "Inst/Import.hpp"
#include "Inst/Throw.hpp"
#include "Inst/Var.hpp"
#include "Inst/Set.hpp"
#include "Inst/End.hpp"
#include "Inst/If.hpp"
#include "Inst/While.hpp"
#include "Inst/Func.hpp"
#include "Inst/Return.hpp"


const std::unordered_map<std::string, const Instruction*> INSTRUCTIONS = {
	{"import",   &INST_Import},
	{"merge",    &INST_Import},
	{"throw",    &INST_Throw},
	{"var",      &INST_Var},
	{"const",    &INST_Var},
	{"arg",      &INST_Var},
	{"set",      &INST_Set},
	{"/",        &INST_End},
	{"if",       &INST_If},
	{"elif",     &INST_If},
	{"else",     &INST_If},
	{"while",    &INST_While},
	{"func",     &INST_Func},
	{"return",   &INST_Return},
};
const std::vector<std::string> declarative_instructions = {"var","const","func"};


Variant last_expr_result;


struct CompositeItem {
	InstToken token;
	unsigned int index = 0;
	uint16_t size = 0;
	unsigned int ln = 0;
	unsigned int col = 0;
	bool declarative = false;
};


std::vector<InstToken> ity_tokenize(const std::string& src) {
	const size_t& src_len = src.size();
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
	CompositeItem last_comp_item;
	uint16_t last_comp_item_dist = 0;

	for (size_t i = 0; i < src_len; i++) {
		const char& ch = src[i];
		// Advance column or line number.
		col++;
		if(ch == '\n') {
			ln++;
			col = 0;
		}

		// End comment.
		if (is_comment) {
			if (ch == '\n') is_comment = false;
			continue;
		}

		// Skip over spaces & tabs at the start of the item.
		if (is_start) {
			if (ch == ' ' || ch == '\n' || ch == '\t') continue;
			// Start comment.
			if (ch == '#') {
				is_comment = true;
				continue;
			}
			item = InstToken{(unsigned int)sequence.size(), ln,col};
			is_start = false;
		}

		if (is_string) {
			// End escaped.
			if (is_escaped_char) {
				is_escaped_char = false;
				buffer.push_back(ch);
				continue;
			}
			// Start escaped.
			if (ch == '\\') {
				is_escaped_char = true;
				buffer.push_back(ch); // Keep backslash, in case it is needed for an expression.
				continue;
			}
			// End string.
			if (ch == string_type) {
				is_string = false;
				buffer.push_back(ch);
				continue;
			}
		}

		else {
			// Start string.
			if (ch == '\'' || ch == '"') {
				is_string = true;
				string_type = ch;
				str_start_ln = ln;
				str_start_col = col;
				buffer.push_back(ch);
				continue;
			}

			// Add argument to args.
			if (ch == ' ' || ch == '\n') {
				if (buffer.size() > 0) {
					item.args.push_back(buffer);
					buffer.clear();
				}
				continue;
			}

			// If is instruction end, append item to sequence & reset state.
			if (ch == ';') {
				last_comp_item_dist += 1;

				// Append remaining argument to args.
				if (buffer.size() > 0) {
					item.args.push_back(buffer);
					buffer.clear();
				}
				// Handle composite instructions.
				for (CompositeItem& comp_item : composite_nest) {
					// Throw error if composite size is about to go over the max for a 16-bit unsigned integer.
					if (comp_item.size == uint16_max) {
						emit_error(ERR_max_composite_size, {}, ln,col);
						return sequence;
					}
					comp_item.size += 1;
					// Flag composite as declarative, if a declarative instruction is found inside it.
					if (&comp_item == &composite_nest.back() && item.args.size() > 0 && exists_in_vec(declarative_instructions, item.args[0])) comp_item.declarative = true;
				}
				if (item.args.size() > 0) {
					const std::string& inst_name = item.args[0];
					if (INSTRUCTIONS.find(inst_name) != INSTRUCTIONS.end()) {
						// Link elif / else.
						if (inst_name == "elif" || inst_name == "else") {
							if (last_comp_item.token.args[0] != "if" && last_comp_item.token.args[0] != "elif") {
								emit_error(ERR_unexpected_else, {}, ln,col);
								return sequence;
							}
							item.linked_inst = last_comp_item.token.args[0];
							item.linked_inst_pos = -(int32_t)last_comp_item_dist;
						}

						// Check if return has a parent function.
						else if (inst_name == "return") {
							bool found = false;
							std::vector<CompositeItem> reverse_nest = composite_nest; std::reverse(reverse_nest.begin(), reverse_nest.end());
							for (const CompositeItem& comp_item : reverse_nest) {
								if (comp_item.token.args[0] != "func") continue;
								found = true;
								break;
							}
							if (not found) {
								emit_error(ERR_unexpected_return, {}, ln,col);
								return sequence;
							}
						}

						// Start composite item...
						const Instruction* inst = INSTRUCTIONS.at(inst_name);
						if (inst->is_composite) {
							composite_nest.push_back(CompositeItem{item, (unsigned int)sequence.size(), 0, ln, col});
						}
						// End composite item...
						else if (inst_name == "/") {
							if (composite_nest.size() > 0) {
								// Get & remove most recent composite item.
								CompositeItem comp_item = composite_nest.back();
								composite_nest.pop_back();
								// Apply updated token to sequence.
								comp_item.token.composite_size = comp_item.size;
								comp_item.token.declarative_composite = comp_item.declarative;
								if (comp_item.token.args[0] == "func") {
									comp_item.token.meta = {(unsigned int)sequence.size()};
								}
								sequence[comp_item.index] = comp_item.token;
								item.linked_inst = comp_item.token.args[0];
								item.linked_inst_pos = -comp_item.size;
								item.declarative_composite = comp_item.declarative; // End instruction should also easily know if the composite is declarative.
								// Save composite item for later.
								last_comp_item = comp_item;
								last_comp_item_dist = -item.linked_inst_pos;
							}
							// Throw error if there is no composite to end.
							else {
								emit_error(ERR_no_composite_requiring_end, {}, ln,col);
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

		buffer.push_back(ch);
	}

	// Throw error if unterminated string.
	if (is_string) {
		emit_error(ERR_no_string_end, {}, str_start_ln, str_start_col);
	}
	// Throw error if unterminated composite.
	if (composite_nest.size() > 0) {
		const CompositeItem& comp_item = composite_nest[composite_nest.size()-1];
		emit_error(ERR_no_composite_end, {}, comp_item.ln, comp_item.col);
	}
	if (debug_flags.inst_seq) {
		std::cout << ANSI::purple << "Instruction Sequence: " << ANSI::reset << sequence << '\n';
	}

	return sequence;
}




// Execute a sequence of instruction tokens.
void ity_exec(ScopeState& state, std::vector<InstToken>& sequence, const size_t start_idx, const int end_idx) {
	InstTokenSeq = sequence;
	execution_depth += 1;
	const size_t seq_len = InstTokenSeq.size();
	for (size_t i = start_idx; i < seq_len; i++) {
		if ((int)i == end_idx) break;

		InstToken& item = InstTokenSeq[i];
		current_line = item.ln;
		current_column = item.col;
		current_inst_token_col = item.col;
		current_inst_token_args = item.args;
		const int args_len = item.args.size();
		if (args_len == 0) continue;

		// If not matched any instruction, run as expression.
		if (INSTRUCTIONS.find(item.args[0]) == INSTRUCTIONS.end()) {
			last_expr_result = expr_run(state, join_str(item.args, " "));
		}
		// Execute instruction.
		else {
			const Instruction* inst = INSTRUCTIONS.at(item.args[0]);
			if (args_len < inst->REQUIRED) {
				emit_error(ERR_invalid_inst_arg_count, {item.args[0], std::to_string(inst->REQUIRED)});
				return;
			}
			inst->exec(state, inst, item, item.args);
			if (exec_jump_value != 0) {
				i += exec_jump_value;
				exec_jump_value = 0;
			}
			if (exec_jump_out) {
				exec_jump_out = false;
				break;
			}
		}
	}
	execution_depth -= 1;
}




int main(int argc, char *argv[]) {
	// Initialize Ity.
	Ity.tokenize = ity_tokenize;
	Ity.exec = ity_exec;

	// Get command line arguments & interpreter flags...
	ARR_t script_args;
	std::vector<std::string> flags;
	std::string source_script_path = "";
	for (int i = 1; i < argc; i++) {
		const std::string& arg_str (argv[i]);
		if (arg_str[0] == '-') flags.push_back(arg_str);
		else if (source_script_path.empty()) source_script_path = arg_str;
		else script_args.push_back(Variant{STR, arg_str, VariantMode_constant});
	}

	// Set debug flags
	debug_flags.result = exists_in_vec(flags, "-d-result");
	debug_flags.inst_seq = exists_in_vec(flags, "-d-inst-seq");
	debug_flags.expr_seq = exists_in_vec(flags, "-d-expr-seq");
	debug_flags.expr_result = exists_in_vec(flags, "-d-expr-result");
	debug_flags.data_assign = exists_in_vec(flags, "-d-data-assign");
	debug_flags.scoping = exists_in_vec(flags, "-d-scoping");
	if (exists_in_vec(flags, "-d-full")) {
		debug_flags.result = true;
		debug_flags.inst_seq = true;
		debug_flags.expr_seq = true;
		debug_flags.expr_result = true;
		debug_flags.data_assign = true;
		debug_flags.scoping = true;
	}

	// Set other flags.
	emit_just_codes = exists_in_vec(flags, "-codes");
	emit_warnings = not exists_in_vec(flags, "-nowarn");

	std::vector<std::string> split_source_script = {""};
	for (const std::string& i : (split_str(source_script_path, '/')) ) {split_source_script.push_back(i);}


	// Initialize state.
	ScopeState state = create_new_scope_state({
		{"__VERSION__",                Variant{ARR, (ARR_t){Variant{INT,ItyVersion[0]}, Variant{INT,ItyVersion[1]}, Variant{INT,ItyVersion[2]}, Variant{INT,ItyVersion[3]}}, VariantMode_constant}},
		{"__VERSION_STRING__",         Variant{STR, ItyVersionString, VariantMode_constant}},
		{"__OS_NAME__",                Variant{STR, OSName, VariantMode_constant}},
		{"__SCRIPT_FILE_NAME__",       Variant{STR, split_source_script.back(), VariantMode_constant}},
		{"__SCRIPT_START_TIME_MS__",   Variant{INT, (int)std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now().time_since_epoch()).count(), VariantMode_constant}},
		{"__CMD_ARGS__",               Variant{ARR, script_args, VariantMode_constant}}
	});
	// Merge MiscBuiltin module.
	LIB_BI_init(state, (ARR_t){});
	merge_module(state, std::any_cast<MAP_t>(LIB_BI.d));

	std::vector<Clock_t> timers = {Clock::now(), Clock::now()};


	// Parse & execute script file...
	if (not source_script_path.empty()) {
		if (not str_ends_with(source_script_path,".ity")) {
			emit_error(ERR_expected_ity_extension);
			return 0;
		}
		std::ifstream f (source_script_path, std::ios::in | std::ios::binary);
		if (not f.is_open()) {
			emit_error(ERR_unable_to_open_script, {source_script_path});
			return 0;
		}

		current_line = 1;
		current_column = 1;
		clock_start = Clock::now();

		// Tokenize the script.
		timers[0] = Clock::now();
		std::vector<InstToken> sequence = Ity.tokenize((std::ostringstream() << f.rdbuf()).str());
		f.close();
		timers[1] = Clock::now();

		// Execute tokens.
		Ity.exec(state, sequence, 0,-1);
	}


	// Run interactive interpreter...
	else {
		std::cout << "* " << ANSI::yellow << "Ity (" << ItyVersionString << ")" << ANSI::reset << '\n'
			      << "* " << ANSI::purple << "Running interactive mode interpreter." << ANSI::reset << '\n'
			      << "* " << ANSI::purple << "Type \"quit\" or \"q\" to stop." << ANSI::reset << '\n';

		current_line = 1;
		current_column = 1;
		clock_start = Clock::now();

		// Input loop...
		std::string command;
		while (true) {
			std::cout << ANSI::purple << "\n>> " << ANSI::reset;
			std::getline(std::cin, command);
			if (command == "quit" || command == "q") {
				break;
			}
			else {
				command += ';';
				last_expr_result = VariantPresets.empty;

				// Tokenize the command.
				std::vector<InstToken> sequence = Ity.tokenize(command);
				// Execute tokens.
				Ity.exec(state, sequence, 0,-1);

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
		const auto total_end = std::chrono::high_resolution_clock::now();
		const std::vector<std::vector<long int>> times = {
			{std::chrono::duration_cast<std::chrono::microseconds>(total_end-clock_start).count(), std::chrono::duration_cast<std::chrono::milliseconds>(total_end-clock_start).count()},
			{std::chrono::duration_cast<std::chrono::microseconds>(timers[1]-timers[0]).count(), std::chrono::duration_cast<std::chrono::milliseconds>(timers[1]-timers[0]).count()},
		};
		std::cout << "\n\n" << "Program results...\n------------------\n";
		if (not source_script_path.empty()) {std::cout << "TIME (Inst-Tokenization): " << std::to_string(times[1][1]/1000.0) << "s (" << times[1][0] << "us).\n";}
		std::cout <<                                      "TIME (total):             " << std::to_string(times[0][1]/1000.0) << "s (" << times[0][0] << "us).\n";
		std::cout <<                                      "STATE SIZE:               " << get_state_size(state) << " bytes." << '\n';
		std::cout << '\n';
	}


	return 0;
}
