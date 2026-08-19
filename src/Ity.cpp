#include <string>
#include <vector>
#include <unordered_map>

#include <iostream>
#include <fstream>
#include <chrono>
#include <filesystem> // NOTE: "filesystem" is quirky across translation units, it is crucial it stays in the main file, otherwise we will get "std::bad_alloc" exceptions when trying to use it.

#include "Registry.hpp"
#include "Util.hpp"
#include "ScriptErrors.hpp"
#include "Variant.hpp"

const void* LIB_BI_G = nullptr; // Global reference to built-in lib.
ARR_t ARGS;

#include "Common.hpp"
#include "State.hpp"
#include "Ity.hpp"
const unsigned int RANDOM_SEED = DurCast_ms(Clock::now() - std::chrono::time_point<std::chrono::high_resolution_clock>()).count();
#include "Expression.hpp"

// Lib imports...
#include "Lib//BuiltIn/m.hpp"
//BUILDER_INSERT: Lib Includes

const Variant LIBS[] = {
	LIB_BI,
//BUILDER_INSERT: Lib Names
};


// Instruction imports...
#pragma pack(1)
struct InstDef {
	const InstSymbol sym;
	const std::string str;
	const Instruction* inst;
};
const InstDef* find_InstDef(const InstSymbol& sym, const std::string& str);
#include "Inst/Import.hpp"
#include "Inst/Exit.hpp"
#include "Inst/Var.hpp"
#include "Inst/End.hpp"
#include "Inst/If.hpp"
#include "Inst/Loop.hpp"
#include "Inst/Continue.hpp"
#include "Inst/Func.hpp"
#include "Inst/Return.hpp"

const InstDef INSTRUCTIONS[] = {
	{InstSymbol_import,   "import",    INST_Import},
	{InstSymbol_merge,    "merge",     INST_Import},

	{InstSymbol_exit,     "exit",      INST_Exit},
	{InstSymbol_throw,    "throw",     INST_Exit},

	{InstSymbol_var,      "var",       INST_Var},
	{InstSymbol_const,    "const",     INST_Var},
	{InstSymbol_arg,      "arg",       INST_Var},

	{InstSymbol_end,      "/",         INST_End},
	{InstSymbol_if,       "if",        INST_If},
	{InstSymbol_elif,     "elif",      INST_If},
	{InstSymbol_else,     "else",      INST_If},

	{InstSymbol_while,    "while",     INST_Loop},
	{InstSymbol_for,      "for",       INST_Loop},
	{InstSymbol_continue, "continue",  INST_Continue},
	{InstSymbol_break,    "break",     INST_Continue},

	{InstSymbol_func,     "func",      INST_Func},
	{InstSymbol_return,   "return",    INST_Return},
};
// Get string representation of an InstSymbol.
const InstDef* find_InstDef(const InstSymbol& sym = InstSymbol__, const std::string& str = "") {
	for (const InstDef& def : INSTRUCTIONS) {
		if ((sym != InstSymbol__ && def.sym == sym)
		|| (not str.empty() && def.str == str)) return &def;
	}
	return nullptr;
}


namespace Ity {


void init() {
	LIB_BI_G = &LIB_BI;
}


std::vector<InstToken> tokenize(const std::string& src) {
	const size_t& src_len = src.size();
	std::vector<InstToken> sequence;
	std::string buffer; buffer.reserve(src_len);
	InstToken item;
	unsigned int ln = 1;
	unsigned int col = 0;

	bool is_start = true;
	bool is_comment = false;
	bool is_string = false;
	bool is_escaped_char = false;
	unsigned int str_start_ln = ln;
	unsigned int str_start_col = col;
	char string_type;

	std::vector<CompositeItem> composite_nest;
	CompositeItem last_comp_item;
	UINT_t last_comp_item_dist = 0;

	for (size_t i=0; i<src_len; i++) {
		const char& ch = src[i];
		LN_COL_COUNTER(ch,ln,col);

		// End comment.
		if (is_comment) {
			if (ch == '\n') is_comment = false;
			continue;
		}

		// Start comment.
		if (ch == COMMENT_SYMBOL && not is_string) {
			is_comment = true;
			continue;
		}

		// Skip over spaces & tabs at the start of the item.
		if (is_start) {
			if (ch == ' ' || ch == '\n' || ch == '\t') continue;
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
			if (STRING_SYMBOLS.find(ch) != std::string::npos) {
				is_string = true;
				string_type = ch;
				str_start_ln = ln;
				str_start_col = col;
				buffer.push_back(ch);
				continue;
			}

			// Add argument to args.
			if (ch == ' ' || ch == '\n' || ch == '\t') {
				if (not buffer.empty()) item.args.push_back(buffer);
				buffer.clear();
				continue;
			}

			// If is instruction end, append item to sequence & reset state.
			if (ch == INST_END_SYMBOL) {
				last_comp_item_dist += 1;

				// Append remaining argument to args.
				if (buffer.size() > 0) {
					item.args.push_back(buffer);
					buffer.clear();
				}

				const size_t& args_len = item.args.size();
				InstSymbol inst_symbol = InstSymbol__;
				if (args_len > 0) {
					if (const InstDef* inst_def = find_InstDef(InstSymbol__, item.args[0]); inst_def != nullptr) inst_symbol = inst_def->sym;
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
					if (&comp_item == &composite_nest.back() && args_len > 0 && exists_in_arr(DECL_INSTRUCTIONS, DECL_INSTRUCTIONS_size, inst_symbol)) comp_item.declarative = true;
				}
				if (args_len > 0) {
					// If is a valid instruction...
					const InstDef* inst_def = find_InstDef(inst_symbol);
					if (inst_def) {
						const Instruction* inst = inst_def->inst;
						item.symbol = inst_symbol;
						item.inst = inst;

						// Check arguments.
						if (args_len < inst->REQUIRED) {
							emit_error(ERR_invalid_inst_arg_count, {item.args[0], std::to_string(inst->REQUIRED)}, ln,col);
							return sequence;
						}

						// Call token processor.
						if (inst->processor) {;
							inst->processor(item, (AnyMap_t){
								{"lcid",  last_comp_item_dist},
								{"lci",   &last_comp_item},
								{"cn",    &composite_nest}
							}, ln,col);
						}

						// Tokenize expression if possible...
						if (inst->has_expr) {
							item.args = tokenize_expr_from_inst_args(item, inst->REQUIRED);
						}

						// Start composite item...
						if (inst->is_composite) {
							composite_nest.push_back(CompositeItem{item, (unsigned int)sequence.size(), 0, ln, col});
						}
						// End composite item...
						else if (inst_symbol == InstSymbol_end) {
							if (composite_nest.size() > 0) {
								// Get & remove most recent composite item.
								CompositeItem comp_item = composite_nest.back();
								composite_nest.pop_back();
								// Apply updated token to sequence.
								comp_item.token.composite_size = comp_item.size;
								comp_item.token.declarative_composite = comp_item.declarative;
								if (comp_item.token.symbol == InstSymbol_func) {
									comp_item.token.meta = {(unsigned int)sequence.size()};
								}
								sequence[comp_item.index] = comp_item.token;
								item.linked_inst = comp_item.token.symbol;
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
					// Instruction is a standalone expression.
					else {
						std::string expr_string; expr_string.reserve(item.args.size());
						unsigned int i_ = 0; for (const std::string& arg : item.args) {
							if (i_ > 0) expr_string += ' ';
							expr_string += arg;
							i_++;
						}
						item.expr = expr_tokenize(std::move(expr_string), item.ln, item.col-1);
						item.args.clear();
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
	if (is_string) emit_error(ERR_no_string_end, {}, str_start_ln, str_start_col);
	// Throw error if unterminated composite.
	if (composite_nest.size() > 0) {
		const CompositeItem& comp_item = composite_nest[composite_nest.size()-1];
		emit_error(ERR_no_composite_end, {}, comp_item.ln, comp_item.col);
	}

	// Clear expression cache in `Expression.hpp`, we no longer need it all the expressions have been tokenized.
	expr_cache.clear();

	return sequence;
}




// Execute a sequence of instruction tokens.
void exec(ItyState& state, const size_t start_idx, const int end_idx) {
	current_script_path = state.path;
	execution_depth += 1;
	const size_t seq_len = (end_idx > 0)
		? std::min((int)state.seq.size(), end_idx+1)
		: state.seq.size()
	;
	for (size_t i = start_idx; i < seq_len; i++) {
		InstToken& item = state.seq[i];
		current_line = item.ln;
		current_column = item.col;

		// If in step mode, print token & wait for confirmation before continuing.
		#ifdef RUNTIME_DEBUG
		if (debug_flags.inst) {
			std::cout << ANSI::purple << "InstToken: " << ANSI::reset << item << '\n';
		}
		if (step_mode) {
			std::cout << ANSI::orange << item << ANSI::reset;
			std::string _input; std::getline(std::cin, _input);
		}
		#endif

		// Run as expression if not an instruction.
		if (not item.inst) {
			state.last_expr_result = *expr_exec(state, item.expr, false);
			continue;
		}

		// Execute the instruction.
		item.inst->exec(state, item);
		// Skip specified number of instructions if `exec_jump_value` has been set.
		if (state.exec_jump_value != 0) {
			i += state.exec_jump_value;
			state.exec_jump_value = 0;
		}
		// Break execution loop, if `exec_jump_out` has been set to true.
		if (state.exec_jump_out) {
			state.exec_jump_out = false;
			break;
		}
	}
	execution_depth -= 1;
}




void start_shell(int argc, char* argv[]) {
	// Get command line arguments & interpreter flags...
	ARR_t script_args;
	std::string source_script_path = "";


	// Iterate arguments.
	for (int i = 1; i < argc; i++) {
		const std::string& s = argv[i];

		// Append to script arguments.
		if (not source_script_path.empty()) {
			script_args.push_back(Variant{STR, (STR_t)s, VariantMode_constant});
		}

		// Parse flags.
		else if (s[0] == '-') {
			// Version.
			if (s == "-v" || s == "--version") {
				std::cout << "Ity " << ItyVersionString << '\n';
				return;
			}
			// Help.
			else if (s == "-h" || s == "--help") {
				std::cout << "Haha no. Go to github.com/phosxd/Ity for now.\n";
				return;
			}
			// Set debug flags.
			else if (s == "--d-result")            debug_flags.result = true;
			#ifdef RUNTIME_DEBUG
			else if (s == "--d-inst")         debug_flags.inst = true;
			else if (s == "--d-expr")         debug_flags.expr = true;
			else if (s == "--d-expr-result")  debug_flags.expr_result = true;
			else if (s == "--d-data-assign")  debug_flags.data_assign = true;
			else if (s == "--d-scoping")      debug_flags.scoping = true;
			else if (s == "-d" || s == "--d-full") {
				debug_flags.result = true;
				debug_flags.inst = true;
				debug_flags.expr = true;
				debug_flags.expr_result = true;
				debug_flags.data_assign = true;
				debug_flags.scoping = true;
			}
			else if (str_starts_with(s, std::string("-t=")) || str_starts_with(s, std::string("--tabs="))) {
				tab_col_value = std::stoi(s.substr(s.size()-1));
			}
			#endif
			// Set other flags.
			else if (s == "-c" || s == "--codes")   emit_just_codes = true;
			else if (s == "--nowarn")               emit_warnings = false;
			else if (s == "--safe")                 safe_mode = true;
			else if (s == "-s" || s == "--step")    step_mode = true;
		}

		// Set script path.
		else if (source_script_path.empty()) source_script_path = s;
	}

	current_script_path = source_script_path;


	// Initialize state.
	ARGS = script_args;
	ItyState state = ItyState{.path = source_script_path}; state.init();
	std::vector<Clock_t> timers = {Clock::now(), Clock::now()};
	std::srand(RANDOM_SEED);


	// Parse & execute script file...
	if (not source_script_path.empty()) {
		if (not str_ends_with(source_script_path,ITY_FILE_EXT)) {
			emit_error(ERR_expected_ity_extension);
			return;
		}
		std::ifstream f (source_script_path, std::ios::in | std::ios::binary);
		if (not f.is_open()) {
			emit_error(ERR_unable_to_open_script, {source_script_path});
			return;
		}

		current_line = 1;
		current_column = 1;
		clock_start = Clock::now();

		// Tokenize the script.
		timers[0] = Clock::now();
		std::vector<InstToken> sequence = Ity::tokenize((std::ostringstream() << f.rdbuf()).str());
		f.close();
		timers[1] = Clock::now();

		// Execute tokens.
		state.seq = std::move(sequence);
		Ity::exec(state, 0,-1);
	}


	// Run interactive interpreter...
	#ifdef INCLUDE_SHELL
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
			std::cout << ANSI::purple << "\n>> " << ANSI::norm << ANSI::reset;
			std::getline(std::cin, command);
			if (command == "quit" || command == "q") {
				break;
			}

			command += INST_END_SYMBOL;
			state.last_expr_result = VariantPresets.empty;

			// Tokenize the command.
			std::vector<InstToken> sequence = Ity::tokenize(command);
			// Execute tokens.
			state.seq = std::move(sequence);
			Ity::exec(state, 0,-1);

			// Print expression result if there is one.
			if (state.last_expr_result.t != PLACEHOLDER) std::cout << state.last_expr_result;

			current_line += 1;
			current_column = 1;
		}
	}
	#endif


	// Output program results in debug mode.
	#ifdef RUNTIME_DEBUG
	if (debug_flags.result) {
		const auto total_end = std::chrono::high_resolution_clock::now();
		const std::vector<std::vector<long int>> times = {
			{DurCast_us(total_end-clock_start).count(), DurCast_ms(total_end-clock_start).count()},
			{DurCast_us(timers[1]-timers[0]).count(),   DurCast_ms(timers[1]-timers[0]).count()},
		};
		std::cout << "\n\n" << "Program results...\n------------------\n";
		if (not source_script_path.empty()) {std::cout << "TIME (Token):    " << std::to_string(times[1][1]/1000.0) << "s (" << times[1][0] << "us).\n";}
		std::cout <<                                      "TIME (Total):    " << std::to_string(times[0][1]/1000.0) << "s (" << times[0][0] << "us).\n";
		std::cout <<                                      "STATE SIZE:      " << state.scope.get_size() << " bytes." << '\n';
		std::cout <<                                      "STATE MAXID:     " << ItyScope_current_id << '\n';
		std::cout << '\n';
	}
	#endif


	return;
}


}
