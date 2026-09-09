#pragma once

#include <variant>

#define AnyCast(T, var) std::get<T>(var)
#define AnyCastV(T, var) std::get<T>(var)


// Get string representation of a VariantType.
inline const std::string& get_variant_type_name(const VariantType& type) {return VARIANT_TYPE_NAMES.at(type);}


// Get VariantType from a string representation.
inline const VariantType get_variant_type_from_name(const std::string& name) {
	for (const auto& it : VARIANT_TYPE_NAMES) {
		if (it.second == name) return it.first;
	}
	return VT_PLACEHOLDER;
}




// VariantData.
// ------------


// Forward declare these for use inside VariantData.
struct Variant;
struct ItyState;
struct CompositeItem;
struct OpDef;

using INT_t = int64_t;
using UINT_t = uint32_t;
using FLOAT_t = double;
using STR_t = std::string;
using ARR_t = std::vector<Variant>;
using MAP_t = std::unordered_map<STR_t,Variant>;
using NativeFunc_t = Variant(*)(ItyState& state, const ARR_t& args);


#pragma pack(1)
struct FUNC_t {
	// Common parameters...
	VariantType return_type = VT_NONE;
	ARR_t bound_args = {};

	// Script functions only...
	UINT_t token_index = 0;
	UINT_t definition_state_id = 0;
	STR_t script_path = "";

	// Native functions only...
	NativeFunc_t native_callable = nullptr;
};


#pragma pack(1)
struct TREF_t {
	std::string str;
	size_t hash = 0;
};

using VariantData = std::variant<
	Variant*,
	std::monostate,
	bool,
	INT_t,
	UINT_t,
	FLOAT_t,
	STR_t,
	ARR_t,
	MAP_t,
	FUNC_t,

	// Internal types.
	TREF_t,
	size_t,
	VariantType,
	VariantMode,

	CompositeItem*,
	std::vector<CompositeItem>*,
	const OpDef*
>;

using AnyMap_t = std::unordered_map<std::string, VariantData>;



// Variant.
// --------


void emit_operator_overload_error(const std::string& operation, const Variant& a, const Variant& b);


#pragma pack(1)
struct Variant {
	VariantType t = VT_NONE;
	VariantData d = std::monostate();
	VariantMode m = VariantMode_dynamic_type;


	// Return true if `b` is applicable to this variant.
	const bool matches(const Variant& b, const bool do_emit_error = true) const {
		if (b.m == VariantMode_dynamic_type || b.t == t) return true;
		if (do_emit_error) emit_error(ERR_assignment_type_mismatch, {get_variant_type_name(t), get_variant_type_name(b.t)});
		return false;
	}


	// Return the number of bytes that the Variant takes up.
	const size_t get_size() const {
		size_t size = sizeof(t) + sizeof(m);
		switch (t) {
			case VT_INT:    {size += sizeof(AnyCast(INT_t,d));   break;}
			case VT_FLOAT:  {size += sizeof(AnyCast(FLOAT_t,d)); break;}
			case VT_REF:
			case VT_STR:    {size += AnyCast(STR_t,d).size();    break;}

			case VT_ARR: {
				const ARR_t& d_ = AnyCast(ARR_t,d);
				size += sizeof(d_);
				for (const Variant& var : d_) {
					size += var.get_size();
				}
				break;
			}

			case VT_MAP: {
				const MAP_t& d_ = AnyCast(MAP_t,d);
				size += sizeof(d_);
				for (const auto& it : d_) {
					size += it.first.size() + it.second.get_size();
				}
				break;
			}
			default: break;
		}

		return size;
	}


	// Conversion Functions.


	// Get boolean representation.
	bool to_bool() const {
		switch (t) {
			case VT_BOOL:   return AnyCast(bool,d);
			case VT_INT:    return (bool)AnyCast(INT_t,d);
			case VT_FLOAT:  return (bool)AnyCast(FLOAT_t,d);
			case VT_STR:    return AnyCast(STR_t,d) == "true";

			default: return false;
		}
	}


	// Get integer representation.
	INT_t to_int() const {
		switch (t) {
			case VT_BOOL:   return (INT_t)AnyCast(bool,d);
			case VT_INT:    return AnyCast(INT_t,d);
			case VT_FLOAT:  return (INT_t)AnyCast(FLOAT_t,d);
			case VT_STR: {
				const STR_t& d_ = AnyCast(STR_t,d);
				if (d_.size() == 0 || NUM.find(d_[0]) == std::string::npos || not is_int_str_32_in_range(d_)) return 0;
				return std::stoi(d_);
			}

			default: return 0;
		}
	}


	// Get float representation.
	FLOAT_t to_float() const {
		switch (t) {
			case VT_BOOL:   return (FLOAT_t)AnyCast(bool,d);
			case VT_INT:    return (FLOAT_t)AnyCast(INT_t,d);
			case VT_FLOAT:  return AnyCast(FLOAT_t,d);
			case VT_STR: {
				const STR_t& d_ = AnyCast(STR_t,d);
				if (d_.size() == 0 || NUM.find(d_[0]) == std::string::npos) return 0.0;
				return std::stod(d_);
			}

			default: return 0.0;
		}
	}


	// Get string representation.
	STR_t to_str() const {
		switch (t) {
			case VT_OP:   return "OP";
			case VT_TREF: return "TREF:" + AnyCast(TREF_t,d).str;

			case VT_NONE:  return "none";
			case VT_PTR:   return "PTR";
			case VT_REF:   return "REF:" + AnyCast(STR_t,d);
			case VT_BOOL:  return (AnyCast(bool,d) ? "true":"false");
			case VT_UINT:  return std::to_string(AnyCast(UINT_t,d));
			case VT_INT:   return std::to_string(AnyCast(INT_t,d));
			case VT_FLOAT: return std::to_string(AnyCast(FLOAT_t,d));
			case VT_STR:   return AnyCast(STR_t,d);

			case VT_ARR: {
				STR_t buf = "[";
				size_t i = 0;
				for (const Variant& it : AnyCast(ARR_t,d)) {
					if (i != 0) buf += ", ";
					if (it.t == VT_STR) buf += '"' + it.to_str() + '"';
					else buf += it.to_str();
					i++;
				}
				return buf + ']';
			}

			case VT_MAP: {
				STR_t buf = "{";
				size_t idx = 0;
				for (auto& i : AnyCast(MAP_t,d)) {
					if (idx != 0) buf += ", ";
					buf += '"' + i.first + "\": ";
					if (i.second.t == VT_STR) buf += '"' + i.second.to_str() + '"';
					else buf += i.second.to_str();
					idx++;
				}
				return buf + '}';
			}

			case VT_FUNC: {
				const FUNC_t& func = AnyCast(FUNC_t,d);
				return "FUNC:" + std::to_string((uintptr_t)&func.native_callable) + ':' + std::to_string(func.token_index);
			}

			default: return "???";
		}
	}



	// COMPARISON OPERATORS


	const bool operator==(const Variant& b) const {
		switch (t) {
			// If a is bool & b is bool...
			case VT_BOOL: {
				if (b.t == VT_BOOL) return AnyCast(bool,d) == AnyCast(bool,b.d);
				break;
			}
			// If a is int...
			case VT_INT: {
				if (b.t == VT_INT)        return AnyCast(INT_t,d) == AnyCast(INT_t,b.d);
				else if (b.t == VT_FLOAT) return AnyCast(INT_t,d) == AnyCast(FLOAT_t,b.d);
				break;
			}
			// If a is float...
			case VT_FLOAT: {
				if (b.t == VT_INT)        return AnyCast(FLOAT_t,d) == AnyCast(INT_t,b.d);
				else if (b.t == VT_FLOAT) return AnyCast(FLOAT_t,d) == AnyCast(FLOAT_t,b.d);
				break;
			}
			// If a is string & b is string...
			case VT_STR: {
				if (b.t == VT_STR) return AnyCast(STR_t,d) == AnyCast(STR_t,b.d);
				break;
			}
			// If a is array & b is array...
			case VT_ARR: {
				if (b.t == VT_ARR) return AnyCast(ARR_t,d) == AnyCast(ARR_t,b.d);
				break;
			}
			// If a is map & b is map...
			case VT_MAP: {
				if (b.t == VT_MAP) return AnyCast(MAP_t,d) == AnyCast(MAP_t,b.d);
				break;
			}
			default: break;
		}

		// Throw error is none matched.
		emit_operator_overload_error("Compare(==)", *this,b);
		return false;
	}


	const bool operator>(const Variant& b) const {
		switch (t) {
			// If a is int...
			case VT_INT: {
				if (b.t == VT_INT)        return AnyCast(INT_t,d) > AnyCast(INT_t,b.d);
				else if (b.t == VT_FLOAT) return AnyCast(INT_t,d) > AnyCast(FLOAT_t,b.d);
				break;
			}
			// If a is float...
			case VT_FLOAT: {
				if (b.t == VT_INT)        return AnyCast(FLOAT_t,d) > AnyCast(INT_t,b.d);
				else if (b.t == VT_FLOAT) return AnyCast(FLOAT_t,d) > AnyCast(FLOAT_t,b.d);
				break;
			}
			default: break;
		}

		// Throw error is none matched.
		emit_operator_overload_error("Compare(>)", *this,b);
		return false;
	}


	const bool operator<(const Variant& b) const {
		switch (t) {
			// If a is int...
			case VT_INT: {
				if (b.t == VT_INT)        return AnyCast(INT_t,d) < AnyCast(INT_t,b.d);
				else if (b.t == VT_FLOAT) return AnyCast(INT_t,d) < AnyCast(FLOAT_t,b.d);
				break;
			}
			// If a is float...
			case VT_FLOAT: {
				if (b.t == VT_INT)        return AnyCast(FLOAT_t,d) < AnyCast(INT_t,b.d);
				else if (b.t == VT_FLOAT) return AnyCast(FLOAT_t,d) < AnyCast(FLOAT_t,b.d);
			}
			default: break;
		}

		// Throw error is none matched.
		emit_operator_overload_error("Compare(<)", *this,b);
		return false;
	}
};




std::ostream& operator<<(std::ostream& os, const Variant& var) {
	return os << var.to_str();
}


void emit_operator_overload_error(const std::string& operation, const Variant& a, const Variant& b) {
	emit_error(ERR_operand_type_mismatch, {operation, get_variant_type_name(a.t), get_variant_type_name(b.t)});
}


ARR_t operator+(const ARR_t& a, const ARR_t& b) {
	ARR_t result = a; result.reserve(b.size());
	for (size_t i = 0; i < b.size(); i++) {
		result.push_back(b[i]);
	}
	return result;
}


MAP_t operator+(const MAP_t& a, const MAP_t& b) {
	MAP_t result = a; result.reserve(b.size());
	for (const auto& it : b) {
		result[it.first] = it.second;
	}
	return result;
}


Variant operator+(const Variant& a, const Variant& b) {
	switch (a.t) {
		// If a is int...
		case VT_INT: {
			if (b.t == VT_INT)         return Variant{VT_INT,   (AnyCast(INT_t,a.d) + AnyCast(INT_t,b.d))};
			else if (b.t == VT_FLOAT)  return Variant{VT_FLOAT, (AnyCast(INT_t,a.d) + AnyCast(FLOAT_t,b.d))};
		}
		// If a is float...
		case VT_FLOAT: {
			if (b.t == VT_INT)         return Variant{VT_FLOAT, (AnyCast(FLOAT_t,a.d) + AnyCast(INT_t,b.d))};
			else if (b.t == VT_FLOAT)  return Variant{VT_FLOAT, (AnyCast(FLOAT_t,a.d) + AnyCast(FLOAT_t,b.d))};
			break;
		}
		// If a is string...
		case VT_STR: {
			if (b.t == VT_STR)       return Variant{VT_STR, (AnyCast(STR_t,a.d) + AnyCast(STR_t,b.d))};
			else if (b.t == VT_INT)  return Variant{VT_STR, (AnyCast(STR_t,a.d) + char(AnyCast(INT_t,b.d)))}; // Add character to the string, from a code.
			break;
		}
		// If a is array & b is array...
		case VT_ARR: {
			if (b.t == VT_ARR) return Variant{VT_ARR, (AnyCast(ARR_t,a.d) + AnyCast(ARR_t,b.d))};
			break;
		}
		// If a is map & b is map...
		case VT_MAP: {
			if (b.t == VT_MAP) return Variant{VT_MAP, (AnyCast(MAP_t,a.d) + AnyCast(MAP_t,b.d))};
			break;
		}
		default: break;
	}

	// Throw error is none matched.
	emit_operator_overload_error("Arith(+)", a,b);
	return a;
}


Variant operator-(const Variant& a, const Variant& b) {
	switch (a.t) {
		// If a is int...
		case VT_INT: {
			if (b.t == VT_INT)         return Variant{VT_INT,   (AnyCast(INT_t,a.d) - AnyCast(INT_t,b.d))};
			else if (b.t == VT_FLOAT)  return Variant{VT_FLOAT, (AnyCast(INT_t,a.d) - AnyCast(FLOAT_t,b.d))};
			break;
		}
		// If a is float...
		case VT_FLOAT: {
			if (b.t == VT_INT)         return Variant{VT_FLOAT, (AnyCast(FLOAT_t,a.d) - AnyCast(INT_t,b.d))};
			else if (b.t == VT_FLOAT)  return Variant{VT_FLOAT, (AnyCast(FLOAT_t,a.d) - AnyCast(FLOAT_t,b.d))};
			break;
		}
		default: break;
	}

	// Throw error is none matched.
	emit_operator_overload_error("Arith(-)", a,b);
	return a;
}


Variant operator*(const Variant& a, const Variant& b) {
	switch (a.t) {
		// If a is int...
		case VT_INT: {
			if (b.t == VT_INT)         return Variant{VT_INT,   (AnyCast(INT_t,a.d) * AnyCast(INT_t,b.d))};
			else if (b.t == VT_FLOAT)  return Variant{VT_FLOAT, (AnyCast(INT_t,a.d) * AnyCast(FLOAT_t,b.d))};
			break;
		}
		// If a is float...
		case VT_FLOAT: {
			if (b.t == VT_INT)         return Variant{VT_FLOAT, (AnyCast(FLOAT_t,a.d) * AnyCast(INT_t,b.d))};
			else if (b.t == VT_FLOAT)  return Variant{VT_FLOAT, (AnyCast(FLOAT_t,a.d) * AnyCast(FLOAT_t,b.d))};
			break;
		}
		// If a is string & b is int.
		case VT_STR: {
			if (b.t == VT_INT) {
				const INT_t& b_val = AnyCast(INT_t,b.d);
				return Variant{VT_STR, (AnyCastV(STR_t,a.d) * b_val)};
			}
			break;
		}
		// If a is array & b is int.
		case VT_ARR: {
			if (b.t == VT_INT) {
				const INT_t& b_val = AnyCast(INT_t,b.d);
				if (b_val < 1) return Variant{VT_ARR, ARR_t()};
				ARR_t a_val = AnyCastV(ARR_t,a.d);
				ARR_t sum; sum.reserve(a_val.size()*b_val);
				for (INT_t i = 0; i < b_val; i++) {
					for (auto& item : a_val) sum.push_back(item);
				};
				return Variant{VT_ARR, std::move(sum)};
			}
			break;
		}
		default: break;
	}

	// Throw error is none matched.
	emit_operator_overload_error("Arith(*)", a,b);
	return a;
}


Variant operator/(const Variant& a, const Variant& b) {
	switch (a.t) {
		// If a is int...
		case VT_INT: {
			if (b.t == VT_INT) {
				if (const INT_t bd = AnyCast(INT_t,b.d); bd != 0) return Variant{VT_INT, (AnyCast(INT_t,a.d) / bd)};
				emit_error(ERR_zero_division);
			}
			else if (b.t == VT_FLOAT) {
				if (const FLOAT_t bd = AnyCast(FLOAT_t,b.d); bd != 0) return Variant{VT_FLOAT, (AnyCast(INT_t,a.d) / bd)};
				emit_error(ERR_zero_division);
			}
			break;
		}
		// If a is float...
		case VT_FLOAT: {
			if (b.t == VT_INT) {
				if (const INT_t bd = AnyCast(INT_t,b.d); bd != 0) return Variant{VT_FLOAT, (AnyCast(FLOAT_t,a.d) / bd)};
				emit_error(ERR_zero_division);
			}
			else if (b.t == VT_FLOAT) {
				if (const FLOAT_t bd = AnyCast(FLOAT_t,b.d); bd != 0) return Variant{VT_FLOAT, (AnyCast(FLOAT_t,a.d) / bd)};
				emit_error(ERR_zero_division);
			}
			break;
		}
		default: break;
	}

	// Throw error is none matched.
	emit_operator_overload_error("Arith(/)", a,b);
	return a;
}


Variant operator%(const Variant& a, const Variant& b) {
	// If a is int & b is int...
	if (a.t == VT_INT && b.t == VT_INT) {
		if (const INT_t& bd = AnyCast(INT_t,b.d); bd != 0) return Variant{VT_INT, (AnyCast(INT_t,a.d) % bd)};
		emit_error(ERR_zero_division);
	};

	// Throw error is none matched.
	emit_operator_overload_error("Arith(%)", a,b);
	return a;
}
