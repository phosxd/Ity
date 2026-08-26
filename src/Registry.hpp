#pragma once


constexpr std::string ItyVersionString = "0.2.0";
// Last number indicates release type:
//	0 = release.
//	1 = beta / pre-release.
//	2 = experimental / custom.
constexpr uint8_t ItyVersion[4] = {0,2,0, 1};

constexpr std::string OSName =
#if _WIN32
"windows"
#elif __linux__
"linux"
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_MAC
"apple_mac"
#elif TARGET_OS_IPHONE
"apple_iphone"
#endif
#elif __unix__
"unix"
#elif _POSIX_VERSION
"posix"
#else
"unknown"
#endif
;


const std::string GLOBAL_SHARE_PATH =
#if _WIN32
"%LOCALAPPDATA%/ity/"
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_MAC
"~/Library/Application Support/ity/"
#elif TARGET_OS_IPHONE
"Documents/ity/"
#endif
#else
"/usr/local/share/ity/"
#endif
;


constexpr unsigned int uint16_max = 65535;

constexpr char COMMENT_SYMBOL = '#';
constexpr char INST_END_SYMBOL = ';';
constexpr std::string ITY_FILE_EXT = ".ity";

constexpr std::string STRING_SYMBOLS = "'\""; // String identifier symbols.
constexpr std::string MISC_RESERVED_SYMBOLS = "_.,()[]{}@~" + STRING_SYMBOLS; // Symbols reserved for special functionality. Operation symbols should not contain any of these characters.

constexpr unsigned int MAX_TEMPORARY_POOL_RESERVE = 32;



// -------------------------------------------




enum InstSymbol : uint8_t {
	InstSymbol__,

	InstSymbol_import,
	InstSymbol_merge,

	InstSymbol_exit,
	InstSymbol_throw,

	InstSymbol_var,
	InstSymbol_const,
	InstSymbol_arg,

	InstSymbol_end,
	InstSymbol_if,
	InstSymbol_elif,
	InstSymbol_else,

	InstSymbol_while,
	InstSymbol_for,
	InstSymbol_continue,
	InstSymbol_break,

	InstSymbol_func,
	InstSymbol_return,
};


// Declarative instructions.
constexpr size_t DECL_INSTRUCTIONS_size = 6;
constexpr InstSymbol DECL_INSTRUCTIONS[] = {
	InstSymbol_import,
	InstSymbol_merge,

	InstSymbol_var,
	InstSymbol_const,
	InstSymbol_arg,

	InstSymbol_func,
};



// -------------------------------------------



enum ExprTokenType : uint8_t {
	ExprTokenType_variant,
	ExprTokenType_sequence,
};


enum OpSymbol : uint8_t {
	OpSymbol__,

	OpSymbol_add,
	OpSymbol_sub,
	OpSymbol_mul,
	OpSymbol_div,
	OpSymbol_mod,

	OpSymbol_set,
	OpSymbol_add_set,
	OpSymbol_sub_set,
	OpSymbol_mul_set,
	OpSymbol_div_set,
	OpSymbol_mod_set,
	OpSymbol_mov_set,

	OpSymbol_cmp_eq,
	OpSymbol_cmp_neq,
	OpSymbol_cmp_gt,
	OpSymbol_cmp_lt,
	OpSymbol_cmp_gteq,
	OpSymbol_cmp_lteq,
	OpSymbol_cmp_and,
	OpSymbol_cmp_or,

	OpSymbol_ternary,
	OpSymbol_ternary_else,

	OpSymbol_type_cast,
	OpSymbol_access,
};



// -------------------------------------------




enum VariantType : uint8_t {
	// Meta types.
	PLACEHOLDER, // Void, absent.
	INTERNAL,    // Data not meant for in-script usage.
	INFERRED,    // Inferred type. Should not contain data.
	ANY,         // Any type. Should not contain data.
	OP,          // Operation definition pointer.
	TREF,        // Literally typed name reference.
	PTR,         // Variant pointer.
	// Real types.
	REF,         // Name reference.
	NONE,        // Monostate data.
	BOOL,        // Boolean data.
	UINT,        // UInt32 data.
	INT,         // Int32 data.
	FLOAT,       // Float64 data.
	STR,         // String data.
	ARR,         // Array of Variants.
	MAP,         // Unordered String:Variant pairs.
	FUNC,        // Script or native callable.
};


const std::unordered_map<const VariantType, const std::string> VARIANT_TYPE_NAMES = {
	// Meta types.
	{PLACEHOLDER,  "PLACEHOLDER"},
	{INTERNAL,     "INTERNAL"},
	{INFERRED,     "*"},
	{ANY,          "ANY"},
	{OP,           "OP"},
	{TREF,         "TREF"},
	{PTR,          "PTR"},
	// Real types.
	{REF,          "REF"},
	{NONE,         "NONE"},
	{BOOL,         "BOOL"},
	{UINT,         "UINT"},
	{INT,          "INT"},
	{FLOAT,        "FLOAT"},
	{STR,          "STR"},
	{ARR,          "ARR"},
	{MAP,          "MAP"},
	{FUNC,         "FUNC"},
};


enum VariantMode : uint8_t {
	VariantMode_dynamic_type,  // Variant can have any type.
	VariantMode_constant,      // Variant data should never change.
	VariantMode_locked_type,   // Variant type should never change.
};



// -------------------------------------------
