#pragma once


enum ERR_CODE : uint8_t {
	ERR_custom,
	ERR_unexpected,
	ERR_expected_ity_extension,
	ERR_unable_to_open_script,
	ERR_unknown_module,
	ERR_disallowed_member_in_safe_mode,

	// 6
	ERR_max_composite_size,
	ERR_no_composite_requiring_end,
	ERR_no_composite_end,
	ERR_no_string_end,
	ERR_max_execution_depth,

	// 11
	ERR_invalid_syntax,
	ERR_invalid_inst_arg_count,
	ERR_invalid_op,
	ERR_invalid_assignment_op,
	ERR_missing_operand,
	ERR_invalid_cast,
	ERR_unexpected_inst,

	// 18
	ERR_operand_type_mismatch,
	ERR_assignment_type_mismatch,
	ERR_return_type_mismatch,
	ERR_operators_not_allowed,
	ERR_expected_boolean_expression,
	ERR_expected_string_expression,

	// 24
	ERR_name_is_taken,
	ERR_name_is_shadowed,
	ERR_name_must_not_contain_symbols,
	ERR_name_does_not_exist,
	ERR_cannot_initialize_value,
	ERR_cannot_change_constant,
	// 30
	ERR_constant_type_not_explicit,
	ERR_invalid_property_access,
	ERR_index_out_of_range,
	ERR_no_property_with_name,
	ERR_invalid_func_call,
	ERR_invalid_func_arg_count,
	ERR_invalid_func_arg_type,
	ERR_no_args_available,

	// 38
	ERR_unexpected_char_at_expr_end,
	ERR_invalid_character_for_construct,
	ERR_cannot_dereference,
	ERR_max_temporaries_in_use,
};


using Clock = std::chrono::high_resolution_clock;
using Clock_t = std::chrono::time_point<std::chrono::high_resolution_clock>;


namespace ANSI {
	static constexpr std::string reset         = "\e[0m";
	static constexpr std::string bold          = "\e[1m";
	static constexpr std::string black         = "\e[30m";
	static constexpr std::string red           = "\e[31m";
	static constexpr std::string green         = "\e[32m";
	static constexpr std::string orange        = "\e[33m";
	static constexpr std::string blue          = "\e[34m";
	static constexpr std::string purple        = "\e[35m";
	static constexpr std::string white         = "\e[37m";
	static constexpr std::string yellow        = "\e[93m";
	// Extras...
	static constexpr std::string cursor_off    = "\e[?25l";
	static constexpr std::string cursor_on     = "\e[?25h";
	static constexpr std::string clear_screen  = "\e[2J\e[H";
}


struct debug_flags_struct {
	bool result = false;        // Print program results when done.
	bool inst = false;          // Print InstToken sequences directly after tokenization.
	bool expr = false;          // Print ExprToken sequences directly after tokenization.
	bool expr_result = false;   // Print result of ExprToken directly after execution.
	bool data_assign = false;   // Print data being assigned to the current state.
	bool scoping = false;       // Print scope in/out/flush calls.
};

debug_flags_struct debug_flags;




Clock_t clock_start;
unsigned int tab_col_value = 4;
unsigned int current_line = 0;
unsigned int current_column = 0;

// Sequence of line & column numbers relating to the exact location a function was called.
// This is used to show the sequence of function calls leading to an error.
std::vector<unsigned int> call_trace;


constexpr bool has_runtime_debug =
#ifdef RUNTIME_DEBUG
	true
#else
	false
#endif
;

// If true, no fancy messages are displayed, just "ERROR: <code>" or "WARN: <code>".
bool emit_just_codes = false;
// If false, warning messages will not be displayed.
bool emit_warnings = true;
// If true, limits the modules that can be imported.
bool safe_mode = false;
// If true. wait for confirmation to run the next instruction.
bool step_mode = false;




std::string make_err_message(const ERR_CODE code, const std::vector<std::string>& args) {
	switch (code) {
		case ERR_custom:                            return args[0];
		#ifdef RUNTIME_DEBUG
		case ERR_unexpected:                        return "Unexpected (" + args[0] + "): " + args[1] + " Please report bug.";
		#ifdef INCLUDE_SHELL
		case ERR_expected_ity_extension:            return "Expected file with \".ity\" extension.";
		case ERR_unable_to_open_script:             return "Unable to open script at \"" + args[0] + "\".";
		#endif
		case ERR_unknown_module:                    return "No module with name \"" + args[0] + "\" is available.";
		case ERR_disallowed_member_in_safe_mode:    return "Member \"" + args[0] + "\" is not allowed during safe mode (--safe).";

		case ERR_max_composite_size:                return "Exceeded maximum number of instructions under a composite (65,535). Nesting is not healthy.";
		case ERR_no_composite_requiring_end:        return "There is no instruction requiring a composite end here.";
		case ERR_no_composite_end:                  return "Composite instruction has no end.";
		case ERR_no_string_end:                     return "String literal has no end.";
		case ERR_max_execution_depth:               return "Maximum execution depth reached (" + args[0] + "). Use the `set_max_depth` function to increase limit.";

		case ERR_invalid_syntax:                    return "Invalid syntax: " + args[0] + ".";
		case ERR_invalid_inst_arg_count:            return "Invalid number of arguments for \"" + args[0] + "\". Expected at least " + args[1] + " separated by a space.";
		case ERR_invalid_op: {
			std::string part = "\".";
			if (args[0][args[0].size()-1] == '-') {
				part = "\". Hint: isolate negative number with a space (E.g. `1 + -1`, not `1+-1`).";
			}
			return "Invalid operator \"" + args[0] + part;
		}
		case ERR_invalid_assignment_op:             return "Invalid assignment operator \"" + args[0] + "\" for instruction \"" + args[1] + "\".";
		case ERR_missing_operand:                   return "Operator is missing an operand.";
		case ERR_invalid_cast:                      return "Cannot type cast from \"" + args[0] + " to \"" + args[1] + "\".";
		case ERR_unexpected_inst:                   return "Unexpected \"" + args[0] + "\" instruction. No valid previous.";

		case ERR_operand_type_mismatch: {
			std::string part = "\".";
			if (args[2].size() > 0) {
				part = "\" with value of type \"" + args[2] + part;
			}
			return "Cannot perform operation \"" + args[0] + "\" on value of type \"" + args[1] + part;
		}
		case ERR_assignment_type_mismatch:          return "Cannot assign value of type \"" + args[0] + "\" to variable of type \"" + args[1] + "\".";
		case ERR_return_type_mismatch:              return "Cannot return value of type \"" + args[0] + "\" in a function that returns type \"" + args[1] + "\".";
		case ERR_operators_not_allowed:             return "Operators not allowed here. Wrap in grouping instead.";
		case ERR_expected_boolean_expression:       return "Expected a boolean result in expression.";
		case ERR_expected_string_expression:        return "Expected a string result in expression.";

		case ERR_name_is_taken:                     return "Name \"" + args[0] + "\" is already taken within this scope.";
		case ERR_name_is_shadowed:                  return "Shadowed name \"" + args[0] + "\"";
		case ERR_name_must_not_contain_symbols:     return "Name must not contain any symbols. Underscores are allowed.";
		case ERR_name_does_not_exist:               return "Name \"" + args[0] + "\" does not exist.";
		case ERR_cannot_initialize_value:           return "Cannot initialize value \"" + args[0] + "\": " + args[1] + ".";
		case ERR_cannot_change_constant:            return "Cannot change value of constant after declaration.";
		case ERR_constant_type_not_explicit:        return "Constant must have an explicit type, not \"ANY\".";
		case ERR_invalid_property_access:           return "Invalid access to value (" + args[0] + ") using value of type \"" + args[1] + "\".";
		case ERR_index_out_of_range:                return "Access index \"" + args[0] + "\" out of range.";
		case ERR_no_property_with_name:             return "No property named \"" + args[0] + "\" in this object.";
		case ERR_invalid_func_call:                 return "Cannot call function with value of type \"" + args[0] + "\". Wrap arguments in an array.";
		case ERR_invalid_func_arg_count:            return "Function expected " + args[0] + " arguments, not " + args[1] + ".";
		case ERR_invalid_func_arg_type:             return "Function argument " + args[0] + " expected vaue of type \"" + args[1] + "\", not \"" + args[2] + "\"";
		case ERR_no_args_available:                 return "No arguments available in this scope.";

		case ERR_unexpected_char_at_expr_end:       return "Unexpected character \"" + args[0] + "\" at end of expression.";
		case ERR_invalid_character_for_construct:   return "Invalid character for " + args[0] + " construct: \"" + args[1] + "\".";
		case ERR_cannot_dereference:                return "Cannot dereference \"" + args[0] + "\". Not a pointer.";
		case ERR_max_temporaries_in_use:            return "Reduce one-off expression complexity; Maximum number of temporaries in use (" + args[0] + "/" + args[1] + "). This will cause corruption!";
		#endif
		default: break;
	}

	return "";
}


std::string get_script_pos(const std::string& script_name, const unsigned int ln, const unsigned int col) {
	return "(" + script_name + ") Ln/Col " + std::to_string(ln) + ':' + std::to_string(col);
}


void emit_warn(const ERR_CODE code, const std::vector<std::string> args={}) {
	if (not emit_warnings) return;
	if (emit_just_codes) {
		std::cout << "Warning: " << std::to_string(code) << '\n';
		return;
	}

	// Print pretty warning message.
	std::cout << ANSI::yellow << "Warning " << std::to_string(code) << ": " << ANSI::white << make_err_message(code,args) << ANSI::reset << '\n';
	if (current_line != 0 || current_column != 0) std::cout << indent(get_script_pos("", current_line, current_column)) << '\n';
}


void emit_error(const ERR_CODE code, const std::vector<std::string> args={}, unsigned int ln_override=0, unsigned int col_override=0) {
	if (emit_just_codes) {
		std::cout << "Error: " << std::to_string(code) << '\n';
		exit(1);
		return;
	}
	if (ln_override == 0) ln_override = current_line;
	if (col_override == 0) col_override = current_column;

	// Print pretty error message.
	std::cout << ANSI::red << "Error " << std::to_string(code) << ": " << ANSI::white << make_err_message(code,args) << ANSI::reset << '\n';
	if (current_line != 0 || current_column != 0) std::cout << indent(get_script_pos("", ln_override, col_override)) << '\n';

	// Kill program.
	exit(1);
}
