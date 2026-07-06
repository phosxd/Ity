#pragma once


using Clock = std::chrono::high_resolution_clock;
using ClockType = std::chrono::time_point<std::chrono::high_resolution_clock>;


struct ANSI_struct {
	std::string reset = "\033[0m";
	std::string black = "\x1B[30m";
	std::string red = "\x1B[31m";
	std::string green = "\x1B[32m";
	std::string orange = "\x1B[33m";
	std::string blue = "\x1B[34m";
	std::string purple = "\x1B[35m";
	std::string white = "\x1B[37m";
	std::string yellow = "\x1B[93m";
};

struct debug_flags_struct {
	bool result = false;        // Print program results when done.
	bool expr_seq = false;      // Print ExprToken sequences directly after tokenization.
	bool expr_result = false;   // Print result of ExprToken directly after execution.
	bool data_assign = false;   // Print data being assigned to the current state.
};

const ANSI_struct ANSI;
debug_flags_struct debug_flags;


ClockType clock_start;
unsigned int current_line = 1;
unsigned int current_column = 1;

bool debug_mode = false;


std::string err_name_does_not_exist(std::string name) {
	return "Name \"" + name + "\" does not exist.";
}


std::string err_operand_type_mismatch(std::string operation, std::string type_1, std::string type_2) {
	std::string part = "\".";
	if (type_2.size() > 0) {
		part = "\" with value of type \"" + type_2 + part;
	}
	return "Cannot perform operation \"" + operation + "\" on value of type \"" + type_1 + part;
}


std::string err_assignment_type_mismatch(std::string type_1, std::string type_2) {
	return "Cannot assign value of type \"" + type_1 + "\" to variable of type \"" + type_2 + "\".";
}


std::string err_invalid_assignment_op(std::string instruction, std::string op_str) {
	return "Invalid assignment operator \"" + op_str + "\" for instruction \"" + instruction + "\".";
}



std::string get_script_pos(unsigned int ln_override=0, unsigned int col_override=0) {
	if (ln_override == 0) {ln_override = current_line;}
	if (col_override == 0) {col_override = current_column;}
	return "Ln: " + std::to_string(ln_override) + ", Col: " + std::to_string(col_override);
}


void emit_warn(std::string message) {
	std::cout << ANSI.yellow << "WARNING at (" << ANSI.orange << get_script_pos() << ANSI.yellow << "): " << ANSI.white << message << ANSI.reset << "\n";
}


void emit_error(std::string message, unsigned int ln_override=0, unsigned int col_override=0) {
	std::cout << ANSI.red << "ERROR at (" << ANSI.orange << get_script_pos(ln_override, col_override) << ANSI.red << "): " << ANSI.white << message << ANSI.reset << "\n";
	exit(1);
}
