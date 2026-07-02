#pragma once

#include <string>
#include <variant>


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
	std::string t;
	std::variant<bool,int,float,std::string> d;
	unsigned int m = 0;
};


std::ostream& operator<<(std::ostream& os, const Variant& s) {
	return os << "{t=" << s.t << ",d=" << s.d << '}';
}
std::ostream& operator<<(std::ostream& os, const std::vector<Variant>& s) {
	os << '[';
	const unsigned int len = s.size();
	for (unsigned int i = 0; i < len; i++) {
		os << s[i] << ", ";
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
// Unordered map.
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




// Instruction.
// ------------

struct Instruction {
	unsigned int REQUIRED = 0;
	int OPTIONAL = 0;
	void (*exec)(ScopeState state, std::vector<std::string> args) = nullptr;
};
