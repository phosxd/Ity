#pragma once


const std::string ALPHA = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"; // Too long to be constexpr.
constexpr std::string NUM = "0123456789";




// Utility Functions.
// ------------------


#define DurCast_us(diff) std::chrono::duration_cast<std::chrono::microseconds>(diff)
#define DurCast_ms(diff) std::chrono::duration_cast<std::chrono::milliseconds>(diff)
#define DurCast_s(diff)  std::chrono::duration_cast<std::chrono::seconds>(diff)
#define DurCast_m(diff)  std::chrono::duration_cast<std::chrono::minutes>(diff)
#define DurCast_h(diff)  std::chrono::duration_cast<std::chrono::hours>(diff)
#define DurCast_d(diff)  std::chrono::duration_cast<std::chrono::days>(diff)
#define DurCast_w(diff)  std::chrono::duration_cast<std::chrono::weeks>(diff)
#define DurCast_M(diff)  std::chrono::duration_cast<std::chrono::months>(diff)
#define DurCast_y(diff)  std::chrono::duration_cast<std::chrono::years>(diff)


#define is_vec_equal(a, b) \
	(a.size() == b.size() \
	&& std::equal(a.begin(), a.end(), b.begin()))


#define str_ends_with(text, suffix) \
	(text.size() >= suffix.size() && (text.compare(text.size()-suffix.size(), suffix.size(), suffix) == 0))


#define str_starts_with(text, prefix) \
	(text.size() >= prefix.size() && (text.compare(0, prefix.size(), prefix) == 0))


// Returns the string with all instances of `ch` removed from the start of it.
constexpr std::string trim_left(const std::string& text, const char ch) {
	const size_t& text_len = text.size();
	if (text_len == 0 || text[0] != ch) return text;
	bool ended = false;
	std::string result; result.reserve(text_len);
	for (size_t i = 0; i < text_len; i++) {
		if (text.at(i) != ch) ended = true;
		if (ended) result.push_back(text[i]);
	}
	return result;
}


// Joins all elements in the vector into a new string, with each element separated by the given `sep`.
constexpr std::string join_str(const std::vector<std::string>& vec, const std::string& sep) {
	const size_t& vec_len = vec.size();
	if (vec_len == 0) return "";
	std::string result = vec.front();
	result.reserve(sep.size() * (vec_len-1));
	for (size_t i = 1; i < vec_len; i++) {
		result += sep + vec[i];
	}
	return result;
}


// Splits the `text` into a vector of strings, with each element separated by the given `sep`.
constexpr std::vector<std::string> split_str(const std::string& text, const char sep) {
	if (text.empty()) return {};

	std::stringstream ss (text);
	std::vector<std::string> result;
	std::string item;
	while (std::getline(ss, item, sep)) {
		result.push_back(item);
	}

	return result;
}


// Return new `text` with each line indented `count` times.
constexpr std::string indent(const std::string& text, const std::string& indent_text="  ") {
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
constexpr unsigned int count_non_empty_strings(const std::vector<std::string>& items) {
	const size_t& items_len = items.size();
	unsigned int count = 0;
	for (size_t i = 0; i < items_len; i++) {
		if (not items[i].empty()) count++;
	}
	return count;
}


// Returns `false` if the string representation falls out of the 32-bit range for integers.
// NOTE: This is not an accurate check, it stops at 2,000,000,000 instead of the actual maximum.
bool is_int_str_32_in_range(std::string int_str) {
	int_str = trim_left(int_str, '0');
	if (int_str.size() == 0) return true;

	const bool negative = (int_str[0] == '-');
	const size_t digits = (negative) ? int_str.size()-1 : int_str.size();
	// If too many or not enough digits, return false.
	if (digits > 10) return false;
	else if (digits < 10) return true;

	uint8_t i = 0;
	if (negative) i++;
	if (int_str[i] == '1') return true;
	return false;
}


template<class T, class T2>
const bool exists_in_vec(const std::vector<T>& v, const T2& val) {
	for (const T& i:v) {
		if (i == val) return true;
	}
	return false;
}


template<class T>
const bool exists_in_arr(const T v[], const size_t& size, const T& val) {
	for (size_t i = 0; i < size; i++) {
		if (v[i] == val) return true;
	}
	return false;
}




// Overloads.
// ----------


// String multiplication.
inline const std::string operator*(const std::string& a, const size_t& b) {
	std::string sum = a; sum.reserve(a.size()*b);
	for (size_t i=1; i<b; i++) sum += a;
	return sum;
}


// Insert uint8_t.
std::ostream& operator<<(std::ostream& os, const uint8_t& s) {
	return os << std::to_string(s); // Convert to string, otherwise it displays as empty.
}


// Insert vector.
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& s) {
	os << '[';
	const unsigned int len = s.size();
	for (unsigned int i = 0; i < len; i++) {
		if (i != 0) os << ", ";
		os << s[i];
	}
	return os << ']';
}


// Insert unordered_map.
template<class T, class T2>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<T,T2>& s) {
	os << '{';
	unsigned int idx = 0;
	for (auto& i:s) {
		if (idx != 0) os << ", ";
		os << "\"" << i.first << "\"" << ": ";
		os << i.second;
		idx++;
	}
	return os << '}';
}
