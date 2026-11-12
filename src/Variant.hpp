#pragma once

#include <variant>

#define AnyCast(T, var) std::get<T>(var)
#define AnyCastV(T, var) std::get<T>(var)
#define AnyCastC(T, var) std::get<T>(var)

// // Gets a constant reference to the value.
// #define AnyCast(T, var) std::any_cast<const T&>(var)
// // Gets a mutable reference to the value.
// #define AnyCastV(T, var) std::any_cast<T&>(var)
// // Gets a copy of the value.
// #define AnyCastC(T, var) std::any_cast<T>(var)


// Get string representation of a VariantType.
#define get_variant_type_name(type) (VARIANT_TYPE_NAMES.at(type)[0])


// Get VariantType from a string representation.
const VariantType get_variant_type_from_name(const std::string& name) {
	if (name == ".") return VT_NONE;
	for (const auto& it : VARIANT_TYPE_NAMES) {
		if (exists_in_vec(it.second, name)) return it.first;
	}
	return VT_PLACEHOLDER;
}


// Get highest order type.
const VariantType get_highest_order_type(const VariantType& a, const VariantType& b) {
	return (a < b) ? a : b; // Enum values are arranged in order of most significant to least.
}


// Type categorization checkers.

const bool type_is_int(const VariantType& t) {
	return t > VT_INT64-1 && t < VT_INT8+1;
}
const bool type_is_uint(const VariantType& t) {
	return t > VT_UINT64-1 && t < VT_UINT8+1;
}
const bool type_is_float(const VariantType& t) {
	return t == VT_FLOAT64;
}
const bool type_is_number(const VariantType& t) {
	return type_is_int(t) || type_is_uint(t) || type_is_float(t);
}




// VariantData.
// ------------


// Forward declare these for use inside VariantData.
struct Variant;
struct ItyState;
struct CompositeItem;
struct OpDef;

using var_key_t = uint32_t;

using FLOAT64_t = double;
using STR_t = std::string;
using ARR_t = std::vector<Variant>;
using MAP_t = std::unordered_map<STR_t,Variant>;
using NativeFunc_t = Variant(*)(ItyState& state, ARR_t& args);


#pragma pack(1)
struct FUNC_t {
	// Common parameters...
	VariantType return_type = VT_NONE;
	ARR_t bound_args = {};

	// Script functions only...
	uint32_t token_index = 0;
	uint32_t definition_state_id = 0;
	STR_t script_path = "";

	// Native functions only...
	NativeFunc_t native_callable = nullptr;
	std::vector<std::vector<VariantType>> args_schema = {};
};


struct TREF_t {
	std::string str;
	var_key_t key = 0;
};

// using VariantData = std::any;
using VariantData = std::variant<
	Variant*,
	std::monostate,
	bool,

	int64_t,
	int32_t,
	int16_t,
	int8_t,
	uint64_t,
	uint32_t,
	uint16_t,
	uint8_t,
	FLOAT64_t,

	STR_t,
	ARR_t,
	MAP_t,
	FUNC_t,

	// Internal types.
	TREF_t,
	VariantType,
	VariantMode,

	CompositeItem*,
	std::vector<CompositeItem>*,
	const OpDef*
>;



// Variant.
// --------


void emit_operator_overload_error(const std::string& operation, const Variant& a, const Variant& b);

#pragma pack(1)
struct Variant {
	VariantType t = VT_NONE;
	VariantData d = std::monostate();
	VariantMode m = VM_dynamic_type;


	template<class T>
	void set(T v) {
		d = v;
	}


	const bool type_mode_matches(const VariantType& a_t, const VariantMode& _a_m, const VariantType& b_t, const VariantMode& b_m, const bool do_emit_error = true) const {
		if (b_m == VM_dynamic_type || b_t == a_t) return true;
		if (do_emit_error) emit_error(ERR_assignment_type_mismatch, {get_variant_type_name(a_t), get_variant_type_name(b_t)});
		return false;
	}
	// Return true if `b` is applicable to this variant.
	const bool matches(const Variant& b, const bool do_emit_error = true) const {
		return this->type_mode_matches(t, m, b.t, b.m, do_emit_error);
	}


	// Return the number of bytes that the Variant takes up.
	const size_t get_size() const {
		return std::visit([](const auto& v) -> size_t {
			using T = std::decay_t<decltype(v)>;
			// Numbers.
			if constexpr (std::is_arithmetic_v<T>) {
				return sizeof(T);
			}
			// String.
			else if constexpr (std::is_same_v<T, STR_t>) {
				return v.size();
			}
			// Array.
			else if constexpr (std::is_same_v<T, ARR_t>) {
				size_t size_ = sizeof(v);
				for (const Variant& var : v) {
					size_ += var.get_size();
				}
				return size_;
			}
			// Map.
			else if constexpr (std::is_same_v<T, ARR_t>) {
				size_t size_ = sizeof(v);
				for (const auto& it : v) {
					size_ += it.first.size() + it.second.get_size();
				}
				return size_;
			}
			else return 0;
		}, d) + sizeof(t) + sizeof(m);
	}


	// CHECKER FUNCTIONS


	bool is_int() const {
		return type_is_int(t);
	}


	bool is_uint() const {
		return type_is_uint(t);
	}


	bool is_float() const {
		return type_is_float(t);
	}


	bool is_number() const {
		return type_is_number(t);
	}


	// DATA COPY CONVERSION FUNCTIONS


	// Get boolean.
	bool to_bool() const {
		return std::visit([](const auto& v) -> bool {
			using VT = std::decay_t<decltype(v)>;
			if constexpr (std::is_arithmetic_v<VT>) return (bool)v;
			else if constexpr (std::is_same_v<VT, STR_t>) return v == "true";
			else return false;
		}, d);
	}


	template<class T>
	T _to_number() const {
		return std::visit([](const auto& v) -> T const {
			using VT = std::decay_t<decltype(v)>;
			if constexpr (std::is_arithmetic_v<VT>) return (T)v;
			else if constexpr (std::is_same_v<VT, STR_t>) {
				if (v.size() == 0 || NUM.find(v[0]) == std::string::npos) return 0;
				else if constexpr (std::is_floating_point_v<T> || std::is_unsigned_v<T>) return (T)std::stod(v);
				else return (T)std::stoll(v);
			}
			else return 0;
		}, d);
	}
	// Get integer.
	int64_t to_int64() const {return _to_number<int64_t>();}
	// Get i32.
	int32_t to_int32() const {return (int32_t)to_int64();}
	// Get i16.
	int16_t to_int16() const {return (int16_t)to_int64();}
	// Get i8.
	int8_t to_int8()   const {return (int8_t)to_int64();}
	// Get unsigned integer.
	uint64_t to_uint64() const {return _to_number<uint64_t>();}
	// Get ui32.
	uint32_t to_uint32() const {return (uint32_t)to_uint64();}
	// Get ui16.
	uint16_t to_uint16() const {return (uint16_t)to_uint64();}
	// Get ui8.
	uint8_t to_uint8()   const {return (uint8_t)to_uint64();}

	// Get float.
	FLOAT64_t to_float64() const {return _to_number<FLOAT64_t>();}


	// Get string.
	STR_t to_str() const {
		switch (t) {
			case VT_OP:   return "OP";
			case VT_TREF: return "TREF:" + AnyCast(TREF_t,d).str;

			case VT_NONE:  return "none";
			case VT_PTR:   return "PTR";
			case VT_REF:   return "REF:" + AnyCast(STR_t,d);
			case VT_BOOL:  return (AnyCast(bool,d) ? "true":"false");

			case VT_INT64:   return std::to_string(AnyCast(int64_t,d));
			case VT_INT32:   return std::to_string(AnyCast(int32_t,d));
			case VT_INT16:   return std::to_string(AnyCast(int16_t,d));
			case VT_INT8:    return std::to_string(AnyCast(int8_t,d));
			case VT_UINT64:  return std::to_string(AnyCast(uint64_t,d));
			case VT_UINT32:  return std::to_string(AnyCast(uint32_t,d));
			case VT_UINT16:  return std::to_string(AnyCast(uint16_t,d));
			case VT_UINT8:   return std::to_string(AnyCast(uint8_t,d));
			case VT_FLOAT64: return std::to_string(AnyCast(FLOAT64_t,d));

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
				return "FUNC:" + get_variant_type_name(func.return_type) + ':' + std::to_string(func.token_index);
			}

			default: return "???";
		}
	}


	// Get array.
	ARR_t to_arr() const {
		switch (t) {
			case VT_ARR: return AnyCast(ARR_t,d);
			default: return (ARR_t){};
		}
	}


	// Get map.
	MAP_t to_map() const {
		switch (t) {
			case VT_MAP: return AnyCast(MAP_t,d);
			default: return (MAP_t){};
		}
	}


	// Get func.
	FUNC_t to_func() const {
		switch (t) {
			case VT_FUNC: return AnyCast(FUNC_t,d);
			default: return (FUNC_t){};
		}
	}


	// FULL CONVERSION FUNCTIONS


	Variant* make_into_type(const VariantType& t_) {
		if (t == t_) return this;
		switch (t_) {
			case VT_BOOL: {d = to_bool(); break;}

			case VT_INT64:   {d = to_int64();   break;}
			case VT_INT32:   {d = to_int32();   break;}
			case VT_INT16:   {d = to_int16();   break;}
			case VT_INT8:    {d = to_int8();    break;}
			case VT_UINT64:  {d = to_uint64();  break;}
			case VT_UINT32:  {d = to_uint32();  break;}
			case VT_UINT16:  {d = to_uint16();  break;}
			case VT_UINT8:   {d = to_uint8();   break;}
			case VT_FLOAT64: {d = to_float64(); break;}

			case VT_STR: {d = to_str(); break;}
			default: {
				emit_error(ERR_invalid_cast, {get_variant_type_name(t), get_variant_type_name(t_)});
				return this;
			};
		} t = t_;
		return this;
	}



	// COMPARISON OPERATORS


	const bool operator==(const Variant& b) const {
		if (t == VT_BOOL && b.t == VT_BOOL) return AnyCast(bool,d) == AnyCast(bool,b.d);

		// Numbers...
		else if (is_int() && b.is_int()) return to_int64() == b.to_int64();
		else if (is_uint() && b.is_uint()) return to_uint64() == b.to_uint64();
		else if (is_number() && b.is_number()) return to_float64() == b.to_float64();

		else if (t == VT_STR && b.t == VT_STR) return AnyCast(STR_t,d) == AnyCast(STR_t,b.d);
		else if (t == VT_ARR && b.t == VT_ARR) return AnyCast(ARR_t,d) == AnyCast(ARR_t,b.d);
		else if (t == VT_MAP && b.t == VT_MAP) return AnyCast(MAP_t,d) == AnyCast(MAP_t,b.d);

		// Throw error is none matched.
		emit_operator_overload_error("Comp(==)", *this,b);
		return false;
	}


	const bool operator!=(const Variant& b) const {
		return not (*this == b);
	}


	const bool operator>(const Variant& b) const {
		// Numbers...
		if (is_int() && b.is_int()) return to_int64() > b.to_int64();
		else if (is_uint() && b.is_uint()) return to_uint64() > b.to_uint64();
		else if (is_number() && b.is_number()) return to_float64() > b.to_float64();

		// Throw error is none matched.
		emit_operator_overload_error("Comp(>)", *this,b);
		return false;
	}


	const bool operator<(const Variant& b) const {
		return not (*this > b) && not (b == *this);
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
	// Numbers...
	if (a.is_int() && b.is_int())            return *Variant{VT_INT64, a.to_int64() + b.to_int64()}.make_into_type(get_highest_order_type(a.t, b.t));
	else if (a.is_uint() && b.is_uint())     return *Variant{VT_UINT64, a.to_uint64() + b.to_uint64()}.make_into_type(get_highest_order_type(a.t, b.t));
	else if (a.is_number() && b.is_number()) return *Variant{VT_FLOAT64, a.to_float64() + b.to_float64()}.make_into_type(get_highest_order_type(a.t, b.t));
	// If both string...
	else if (a.t == VT_STR) {
		if (b.t == VT_STR)         return Variant{VT_STR, (AnyCast(STR_t,a.d) + AnyCast(STR_t,b.d))};
		else if (b.is_number())    return Variant{VT_STR, (AnyCast(STR_t,a.d) + char(b.to_int64()))}; // Add character to the string, from a code.
	}
	// If both array...
	else if (a.t == VT_ARR && b.t == VT_ARR) return Variant(VT_ARR, AnyCast(ARR_t,a.d) + AnyCast(ARR_t,b.d));
	// If both map...
	else if (a.t == VT_MAP && b.t == VT_MAP) return Variant(VT_MAP, AnyCast(MAP_t,a.d) + AnyCast(MAP_t,b.d));

	// Throw error is none matched.
	emit_operator_overload_error("Arith(+)", a,b);
	return a;
}


Variant operator-(const Variant& a, const Variant& b) {
	// Numbers...
	if (a.is_int() && b.is_int())            return *Variant{VT_INT64, a.to_int64() - b.to_int64()}.make_into_type(get_highest_order_type(a.t, b.t));
	else if (a.is_uint() && b.is_uint())     return *Variant{VT_UINT64, a.to_uint64() - b.to_uint64()}.make_into_type(get_highest_order_type(a.t, b.t));
	else if (a.is_number() && b.is_number()) return *Variant{VT_FLOAT64, a.to_float64() - b.to_float64()}.make_into_type(get_highest_order_type(a.t, b.t));

	// Throw error is none matched.
	emit_operator_overload_error("Arith(-)", a,b);
	return a;
}


Variant operator*(const Variant& a, const Variant& b) {
	// Numbers...
	if (a.is_int() && b.is_int())            return *Variant{VT_INT64, a.to_int64() * b.to_int64()}.make_into_type(get_highest_order_type(a.t, b.t));
	else if (a.is_uint() && b.is_uint())     return *Variant{VT_UINT64, a.to_uint64() * b.to_uint64()}.make_into_type(get_highest_order_type(a.t, b.t));
	else if (a.is_number() && b.is_number()) return *Variant{VT_FLOAT64, a.to_float64() * b.to_float64()}.make_into_type(get_highest_order_type(a.t, b.t));
	// If a is string & b is number...
	else if (a.t == VT_STR && b.is_number()) return Variant{VT_STR, (AnyCast(STR_t,a.d) * (uint32_t)b.to_int64())};
	// If a is array & b is number...
	else if (a.t == VT_ARR && b.is_number()) return Variant{VT_ARR, (AnyCast(ARR_t,a.d) * (uint32_t)b.to_int64())};

	// Throw error is none matched.
	emit_operator_overload_error("Arith(*)", a,b);
	return a;
}


Variant operator/(const Variant& a, const Variant& b) {
	if (a.is_number() && b.is_number()) {
		if (const FLOAT64_t& bd = b.to_float64(); bd != 0) return Variant{VT_FLOAT64, (FLOAT64_t)(a.to_float64() / b.to_float64())};
		emit_error(ERR_zero_division);
	}

	// Throw error is none matched.
	emit_operator_overload_error("Arith(/)", a,b);
	return a;
}


Variant operator%(const Variant& a, const Variant& b) {
	if (a.is_number() && b.is_number()) {
		if (const int64_t& bd = b.to_int64(); bd != 0) return Variant{VT_INT64, (a.to_int64() % bd)};
		emit_error(ERR_zero_division);
	};

	// Throw error is none matched.
	emit_operator_overload_error("Arith(%)", a,b);
	return a;
}
