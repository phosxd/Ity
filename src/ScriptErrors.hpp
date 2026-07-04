#pragma once

#include <string>
#include <chrono>


std::chrono::time_point<std::chrono::high_resolution_clock> clock_start;
unsigned int current_line = 0;
unsigned int current_column = 0;


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



std::string get_script_pos() {
	return "Ln: " + std::to_string(current_line) + ", Col: " + std::to_string(current_column);
}


void emit_warn(std::string message) {
	std::cout << "WARNING at (" << get_script_pos() << "): " << message << "\n";
}


void emit_error(std::string message) {
	std::cout << "ERROR at (" << get_script_pos() << "): " << message << "\n";
	exit(1);
}
