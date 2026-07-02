#pragma once

#include <string>
#include <iostream>
#include "../Common.hpp"


Instruction INST_Var {
	4, // "REQUIRED". Type, Name, Operator, & first part of Expression.
	-1, // "OPTIONAL". Other parts of Expression.
};
