#pragma once


constexpr char COMMENT_SYMBOL = '#';
constexpr char INST_END_SYMBOL = ';';
constexpr std::string ITY_FILE_EXT = ".ity";



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


const std::unordered_map<std::string, const InstSymbol> InstSymbolStrs = {
	{"import",     InstSymbol_import},
	{"merge",      InstSymbol_merge},

	{"exit",       InstSymbol_exit},
	{"throw",      InstSymbol_throw},

	{"var",        InstSymbol_var},
	{"const",      InstSymbol_const},
	{"arg",        InstSymbol_arg},

	{"/",          InstSymbol_end},
	{"if",         InstSymbol_if},
	{"elif",       InstSymbol_elif},
	{"else",       InstSymbol_else},

	{"while",      InstSymbol_while},
	{"for",        InstSymbol_for},
	{"continue",   InstSymbol_continue},
	{"break",      InstSymbol_break},

	{"func",       InstSymbol_func},
	{"return",     InstSymbol_return},
};


// Declarative instructions.
constexpr size_t DECL_INSTRUCTIONS_size = 5;
constexpr InstSymbol DECL_INSTRUCTIONS[] = {
	InstSymbol_import,
	InstSymbol_merge,
	InstSymbol_var,
	InstSymbol_const,
	InstSymbol_func,
};



// -------------------------------------------



enum ExprTokenType : uint8_t {
	ExprTokenType_variant,
	ExprTokenType_sequence,
};



// -------------------------------------------




enum VariantType : uint8_t {
	// Meta types.
	PLACEHOLDER, // Void, absent.
	INTERNAL,    // Data not meant for in-script usage.
	INFERRED,    // Inferred type. Should not contain data.
	ANY,         // Any type. Should not contain data.
	OP,          // Operator string identifier.
	TREF,        // Literally typed name reference.
	PTR,         // Variant pointer.
	// Real types.
	REF,         // Name reference.
	NONE,        // Monostate data.
	BOOL,        // Boolean data.
	INT,         // Int32 data.
	FLOAT,       // Float64 data.
	STR,         // String data.
	ARR,         // Array of Variants.
	MAP,         // Unordered String:Variant pairs.
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
	{INT,          "INT"},
	{FLOAT,        "FLOAT"},
	{STR,          "STR"},
	{ARR,          "ARR"},
	{MAP,          "MAP"},
};


enum VariantMode : uint8_t {
	VariantMode_dynamic_type,  // Variant can have any type.
	VariantMode_constant,      // Variant data should never change.
	VariantMode_locked_type,   // Variant type should never change.
};



// -------------------------------------------
