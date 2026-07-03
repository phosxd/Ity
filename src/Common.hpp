#pragma once

#include <string>
#include <variant>
#include <algorithm>


// Types.
// ------

using VariantData = std::variant<bool,int,float,std::string>;



// Prints.
// -------

// vector<string>
std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& s) {
	os << '[';
	const unsigned int len = s.size();
	for(unsigned int i = 0; i < len; i++) {
		os << '"' << s[i] << '"' << ", ";
	}
	return os << ']';
}

// variant<bool,int,float,string>
std::ostream& operator<<(std::ostream& os, const VariantData& s) {
	if (std::holds_alternative<bool>(s)) {
		os << std::get<bool>(s);
	}
	else if (std::holds_alternative<int>(s)) {
		os << std::get<int>(s);
	}
	else if (std::holds_alternative<float>(s)) {
		os << std::get<float>(s);
	}
	else if (std::holds_alternative<std::string>(s)) {
		os << std::get<std::string>(s);
	}
	return os;
}




// Variant.
// --------

struct Variant {
	std::string t = ".";
	std::variant<bool,int,float,std::string> d;
	unsigned int m = 0;
};


std::ostream& operator<<(std::ostream& os, const Variant& s) {
	return os << "{t=" << s.t << ",d=" << s.d << '}';
}
// Vector.
std::ostream& operator<<(std::ostream& os, const std::vector<Variant>& s) {
	os << '[';
	const unsigned int len = s.size();
	for (unsigned int i = 0; i < len; i++) {
		os << s[i] << ", ";
	}
	return os << ']';
}
// Unordered string map.
std::ostream& operator<<(std::ostream& os, const std::unordered_map<std::string,Variant>& s) {
	os << '{';
	const unsigned int len = s.size();
	for (auto i:s) {
		os << i.first << "=" << i.second << ", ";
	}
	return os << ']';
}




// InstToken.
// ----------


struct InstToken {
	unsigned int ln = 0;
	unsigned int col = 0;
	std::vector<std::string> args;
};


std::ostream& operator<<(std::ostream& os, const InstToken& s) {
	return os << "{ln=" << s.ln << ",col=" << s.col << ",args=" << s.args << '}';
}
// Vector.
std::ostream& operator<<(std::ostream& os, const std::vector<InstToken>& s) {
	os << '[';
	const unsigned int len = s.size();
	for (unsigned int i = 0; i < len; i++) {
		os << s[i] << ", ";
	}
	return os << ']';
}
// Unordered string map.
std::ostream& operator<<(std::ostream& os, const std::unordered_map<std::string,InstToken>& s) {
	os << '{';
	const unsigned int len = s.size();
	for (auto i:s) {
		os << i.first << '=' << i.second << ", ";
	}
	return os << '}';
}




// Scope State.
// ------------

struct ScopeState {
	ScopeState* p = nullptr;
	std::unordered_map<std::string,Variant> d;
};


std::ostream& operator<<(std::ostream& os, const ScopeState& s) {
	os << "ScopeState{p=" << s.p << ", " << s.d << "}";
	return os;
}




// Instruction.
// ------------

struct Instruction {
	unsigned int REQUIRED = 0;
	int OPTIONAL = 0;
	void (*exec)(Instruction& inst, ScopeState& state, std::vector<std::string> args) = nullptr;
};

std::ostream& operator<<(std::ostream& os, const Instruction& s) {
	return os << "Instruction{}";
}




// Operation.
// ----------

struct Operation {
	std::unordered_map<std::string,std::vector<std::string>> TYPE_MAP;
	Variant (*exec)(Operation& op, ScopeState& state, Variant& first, Variant& second, std::string symbol) = nullptr;
};




// Utility Functions.
// ------------------

bool exists_in_strvec(std::vector<std::string>& v, std::string val) {
	return (std::find(v.begin(), v.end(), val) != v.end());
}


std::string get_variant_data_type(VariantData& d) {
	if (std::holds_alternative<bool>(d)) {
		return "BOOL";
	}
	else if (std::holds_alternative<int>(d)) {
		return "INT";
	}
	else if (std::holds_alternative<float>(d)) {
		return "FLOAT";
	}
	else if (std::holds_alternative<std::string>(d)) {
		return "STR";
	}
	return "NULL";
}
