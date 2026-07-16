#pragma once


const std::string ALPHA = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";
constexpr std::string NUM = "0123456789";




// Overloads.
// ----------


// String multiplication.
std::string operator*(const std::string& a, const int& b) {
	std::string sum; sum.reserve(a.size()*b);
	for (int i = 0; i < b; i++) {sum += a;}
	return sum;
}



// LShift uint8_t.
std::ostream& operator<<(std::ostream& os, const uint8_t& s) {
	return os << std::to_string(s); // Convert to string, otherwie displays as empty.
}


// LShift vector.
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& s) {
	os << '[';
	const unsigned int len = s.size();
	for (unsigned int i = 0; i < len; i++) {
		if (i != 0) {os << ", ";}
		os << s[i];
	}
	return os << ']';
}


// LShift unordered_map.
template<class T, class T2>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<T,T2>& s) {
	os << '{';
	unsigned int idx = 0;
	for (auto i:s) {
		if (idx != 0) {os << ", ";}
		os << "\"" << i.first << "\"" << ": " << i.second;
		idx++;
	}
	return os << '}';
}




// Utility Functions.
// ------------------


template<class T, class T2>
bool is_vec_equal(const std::vector<T>& a, const std::vector<T2>& b) {
	const size_t& a_len = a.size();
	const size_t& b_len = b.size();
	if (a_len != b_len) {return false;}
	for (size_t i = 0; i < a_len; i++) {
		if (not (a[i] == b[i])) {return false;}
	}
	return true;
}


bool str_ends_with(const std::string& text, const std::string& suffix) {
	return text.size() >= suffix.size() && (text.compare(text.size()-suffix.size(), suffix.size(), suffix) == 0);
}


// Returns the string with all instances of `ch` removed from the start of it.
std::string trim_left(const std::string& text, const char ch) {
	const size_t& text_len = text.size();
	if (text_len == 0) return text;
	if (text.at(0) != ch) return text;
	bool ended = false;
	std::string result; result.reserve(text_len);
	for (size_t i = 0; i < text_len; i++) {
		if (text.at(i) != ch) ended = true;
		if (ended) result.push_back(text.at(i));
	}
	return result;
}


// Joins all elements in the vector into a new string, with each element separated by the given `sep`.
std::string join_str(const std::vector<std::string>& vec, const std::string& sep) {
	const size_t& vec_len = vec.size();
	std::string result = vec.front();
	result.reserve(sep.size() * (vec_len-1));
	for (size_t i = 1; i < vec_len; i++) {
		result += sep + vec.at(i);
	}
	return result;
}


// Splits the `text` into a vector of strings, with each element separated by the given `sep`.
std::vector<std::string> split_str(const std::string& text, const char sep) {
	std::stringstream ss (text);
	std::vector<std::string> result;
	std::string item;
	while (std::getline(ss, item, sep)) {
		result.push_back(item);
	}
	return result;
}


// Return new `text` with each line indented `count` times.
std::string indent(const std::string& text, const std::string& indent_text="  ") {
	std::string result;
	unsigned int i = 0;
	for (const std::string& line : split_str(text, '\n')) {
		if (i != 0) result += '\n';
		result += indent_text + line;
		i++;
	}
	return result;
}


// Returns the number of strings that are empty inside the given vector.
unsigned int count_non_empty_strings(const std::vector<std::string>& items) {
	const size_t& items_len = items.size();
	unsigned int count = 0;
	for (size_t i = 0; i < items_len; i++) {
		if (not std::move(items[i]).empty()) count++;
	}
	return count;
}


// Returns `false` if the string representation falls out of the 32-bit range for integers.
// NOTE: This is not an accurate check, it stops at 2,000,000,000 instead of the actual maximum.
bool is_int_str_32_in_range(std::string int_str) {
	int_str = trim_left(int_str, '0');
	if (int_str.size() == 0) return true;

	const bool negative = (int_str[0] == '-');
	size_t digits = int_str.size();
	if (negative) digits--;
	// If too many or not enough digits, return false.
	if (digits > 10) return false;
	else if (digits < 10) return true;

	uint8_t i = 0;
	if (negative) i++;
	if (int_str[i] == '1') return true;
	return false;
}


template<class T, class T2>
bool exists_in_vec(const std::vector<T>& v, const T2& val) {
	for (const T& i:v) {
		if (i == val) return true;
	}
	return false;
}
