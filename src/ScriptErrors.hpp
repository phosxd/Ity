#pragma once


enum ERR_CODE {
	ERR_unexpected,
	ERR_expected_ity_extension,
	ERR_unable_to_open_script,

	ERR_max_composite_size,
	ERR_no_composite_requiring_end,
	ERR_no_composite_end,
	ERR_no_string_end,

	ERR_invalid_inst_arg_count,
	ERR_invalid_op,
	ERR_invalid_assignment_op,

	ERR_operand_type_mismatch,
	ERR_assignment_type_mismatch,
	ERR_operators_not_allowed,
	ERR_expected_boolean_expression,

	ERR_name_is_taken,
	ERR_name_is_shadowed,
	ERR_name_must_not_contain_symbols,
	ERR_name_does_not_exist,
	ERR_cannot_initialize_value,
	ERR_cannot_change_constant,
	ERR_constant_type_not_explicit,
	ERR_invalid_property_access,
	ERR_index_out_of_range,

	ERR_cannot_multiply_by_negative,

	ERR_unexpected_char_at_expr_end,
	ERR_invalid_character_for_construct,
};

using Clock = std::chrono::high_resolution_clock;
using ClockType = std::chrono::time_point<std::chrono::high_resolution_clock>;


namespace ANSI {
	static std::string reset = "\033[0m";
	static std::string bold = "\033[1m";
	static std::string black = "\x1B[30m";
	static std::string red = "\x1B[31m";
	static std::string green = "\x1B[32m";
	static std::string orange = "\x1B[33m";
	static std::string blue = "\x1B[34m";
	static std::string purple = "\x1B[35m";
	static std::string white = "\x1B[37m";
	static std::string yellow = "\x1B[93m";
}

struct debug_flags_struct {
	bool result = false;        // Print program results when done.
	bool inst_seq = false;      // Print InstToken sequences directly after tokenization.
	bool expr_seq = false;      // Print ExprToken sequences directly after tokenization.
	bool expr_result = false;   // Print result of ExprToken directly after execution.
	bool data_assign = false;   // Print data being assigned to the current state.
};

debug_flags_struct debug_flags;


ClockType clock_start;
unsigned int current_line = 0;
unsigned int current_column = 0;

bool debug_mode = false;



std::string get_script_pos(unsigned int ln_override=0, unsigned int col_override=0) {
	if (ln_override == 0) {ln_override = current_line;}
	if (col_override == 0) {col_override = current_column;}
	return "Ln: " + std::to_string(ln_override) + ", Col: " + std::to_string(col_override);
}


std::string make_err_message(ERR_CODE code, std::vector<std::string> args) {
	if (code == ERR_unexpected) {
		return "Unexpected (" + args[0] + "): " + args[1] + "Please report bug.";
	}
	else if (code == ERR_expected_ity_extension) {
		return "Expected file with \".ity\" extension.";
	}
	else if (code == ERR_unable_to_open_script) {
		return "Unable to open script at \"" + args[0] + "\".";
	}

	else if (code == ERR_max_composite_size) {
		return "Exceeded maximum number of instructions under a composite instruction (65,535). Nesting is not healthy.";
	}
	else if (code == ERR_no_composite_requiring_end) {
		return "There is no instruction requiring a composite end here.";
	}
	else if (code == ERR_no_composite_end) {
		return "Composite instruction has no end.";
	}
	else if (code == ERR_no_string_end) {
		return "String literal has no end.";
	}

	else if (code == ERR_invalid_inst_arg_count) {
		return "Invalid number of arguments for \"" + args[0] + "\". Expected at least " + args[1] + " separated by a space.";
	}
	else if (code == ERR_invalid_op) {
		std::string part = "\".";
		if (args[0][args[0].size()-1] == '-') {
			part = "\". Hint: encapsulate negative number literal in a sub-expression (E.g. `1+(-1)`).";
		}
		return "Invalid operator \"" + args[0] + part;
	}
	else if (code == ERR_invalid_assignment_op) {
		return "Invalid assignment operator \"" + args[0] + "\" for instruction \"" + args[1] + "\".";
	}
	else if (code == ERR_operand_type_mismatch) {
		std::string part = "\".";
		if (args[2].size() > 0) {
			part = "\" with value of type \"" + args[2] + part;
		}
		return "Cannot perform operation \"" + args[0] + "\" on value of type \"" + args[1] + part;
	}
	else if (code == ERR_assignment_type_mismatch) {
		return "Cannot assign value of type \"" + args[0] + "\" to variable of type \"" + args[1] + "\".";
	}
	else if (code == ERR_operators_not_allowed) {
		return "Operators not allowed inside \"" + args[0] + "\". Use sub-expressions instead: " + args[1] + ".";
	}
	else if (code == ERR_expected_boolean_expression) {
		return "Expected a boolean result in expression.";
	}

	else if (code == ERR_name_is_taken) {
		return "Name \"" + args[0] + "\" is already taken within this scope. Use another name for this variable.";
	}
	else if (code == ERR_name_is_shadowed) {
		return "Name \"" + args[0] + "\" is shadowing another variable with the same name, you will not be able to access the shadowed variable unless you change the name.";
	}
	else if (code == ERR_name_must_not_contain_symbols) {
		return "Name must not contain any symbols. Underscores are allowed.";
	}
	else if (code == ERR_name_does_not_exist) {
		return "Name \"" + args[0] + "\" does not exist.";
	}
	else if (code == ERR_cannot_initialize_value) {
		return "Cannot initialize value \"" + args[0] + "\": " + args[1] + ".";
	}
	else if (code == ERR_cannot_change_constant) {
		return "Cannot change value of constant \"" + args[0] + "\" after declaration.";
	}
	else if (code == ERR_constant_type_not_explicit) {
		return "Constant must have an explicit type, not \"ANY\".";
	}
	else if (code == ERR_invalid_property_access) {
		return "Invalid access to value (" + args[0] + ") using value of type \"" + args[1] + "\".";
	}
	else if (code == ERR_index_out_of_range) {
		return "Index \"" + args[0] + "\" out of range. Make sure the item you are accessing has the correct number of elements.";
	}

	else if (code == ERR_cannot_multiply_by_negative) {
		return "Cannot multiply \"" + args[0] + "\"by a negative number.";
	}

	else if (code == ERR_unexpected_char_at_expr_end) {
		return "Unexpected character \"" + args[0] + "\" at end of expression.";
	}
	else if (code == ERR_invalid_character_for_construct) {
		return "Invalid character for " + args[0] + " construct: \"" + args[1] + "\".";
	}

	return "Error code \"" + std::to_string(code) + "\".";
}


void emit_warn(ERR_CODE code, std::vector<std::string> args={}) {
	std::cout << ANSI::yellow << "WARNING at (" << ANSI::orange << get_script_pos() << ANSI::yellow << "): " << ANSI::white << make_err_message(code,args) << ANSI::reset << "\n";
}


void emit_error(ERR_CODE code, std::vector<std::string> args={}, unsigned int ln_override=0, unsigned int col_override=0) {
	std::cout << ANSI::red << "ERROR at (" << ANSI::orange << get_script_pos(ln_override, col_override) << ANSI::red << "): " << ANSI::white << make_err_message(code,args) << ANSI::reset << "\n";
	exit(1);
}
