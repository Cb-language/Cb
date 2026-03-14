#pragma once
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

typedef unsigned char byte;

enum class CbTokenType : byte
{
	// types
	TYPE_FLAT,    // signed
	TYPE_SHARP,   // unsigned
	TYPE_DEGREE,  // int
	TYPE_FREQ,    // float
	TYPE_NOTE,    // char
	TYPE_MUTE,    // bool
	TYPE_BAR,     // string
	TYPE_RIFF,    // array
	TYPE_FERMATA, // void

	// keywords
	KEYWORD_UNISON,         // static
	KEYWORD_BASS,           // ctor in initialize line
	KEYWORD_INSTRUMENT,     // class
	KEYWORD_PLAYERSCORE,    // private
	KEYWORD_CONDUCTORSCORE, // public
	KEYWORD_SECTIONSCORE,   // protected
	KEYWORD_PAUSE,          // break
	KEYWORD_RESUME,         // continue
	KEYWORD_PLAY,           // std::cout
	KEYWORD_PLAYBAR,        // std::cout << std::endl
	KEYWORD_HEAR,           // std::cin
	KEYWORD_SWITCH,         // switch
	KEYWORD_RETURN,         // return
	KEYWORD_CASE,           // case
	KEYWORD_IF,             // if
	KEYWORD_ELSE,           // else
	KEYWORD_FMAJ,           // for loop going up
	KEYWORD_FMIN,           // for loop going down
	KEYWORD_WHILE,          // while
	KEYWORD_SONG,           // func
	KEYWORD_FEAT,           // include
	KEYWORD_MOTIF,          // virtual
	KEYWORD_REST,           // pure virtual
	KEYWORD_VARIATION,      // override
	KEYWORD_CTOR_CALL,      // ctor call
	KEYWORD_C_CLEF,        // 𝄡 (dosent compile)
	KEYWORD_TRANSCRIBE,     // casting
	KEYWORD_TIMBRE,         // is

	// punctuation
	PUNCTUATION_RET_TYPE_ARROW,       // 𝅘𝅥=
	PUNCTUATION_COMMA,                // ,
	PUNCTUATION_COPYRIGHT,            // ©
	PUNCTUATION_OPEN_SQUARE_BRACE,    // [
	PUNCTUATION_CLOSE_SQUARE_BRACE,   // ]
	PUNCTUATION_OPEN_CURLY_BRACKET,   // 𝄋
	PUNCTUATION_CLOSE_CURLY_BRACKET,  // 𝄌
	PUNCTUATION_PARENTHESIS_OPEN,     // 𝄕
	PUNCTUATION_PARENTHESIS_CLOSE,    // 𝄇
	PUNCTUATION_OPEN_TRIANGLE_BRACE,  // 𝆒
	PUNCTUATION_CLOSE_TRIANGLE_BRACE, // 𝆓
	PUNCTUATION_COLON,                // :
	PUNCTUATION_BACKSLASH,            // 𝄍
	PUNCTUATION_DOUBLE_BACKSLASH,     // 𝄓
	PUNCTUATION_REST,                 // 𝄽
	PUNCTUATION_OPEN_LINE,            // 𝄞
	PUNCTUATION_SEMICOLON,            // 𝄀
	PUNCTUATION_CLOSE_FUNC,           // 𝄂
	PUNCTUATION_NEW_LINE,             // \n

	// unary operations
	UNARY_OP_SHARP,        // ♯    ++
	UNARY_OP_DOUBLE_SHARP, // 𝄪     += 2
	UNARY_OP_FLAT,         // ♭     --
	UNARY_OP_DOUBLE_FLAT,  // 𝄫     -= 2
	UNARY_OP_NOT,          // !
	UNARY_OP_NATRUAL,      // ♮      =0

	// binary operation
	BINARY_OP_EQUAL,         // =
	BINARY_OP_EQUALITY,      // ==
	BINARY_OP_NOT_EQUAL,     // !=
	BINARY_OP_LESSER_EQUAL,  // <=
	BINARY_OP_GREATER_EQUAL, // >=
	BINARY_OP_PLUS_EQUAL,     // +=
	BINARY_OP_MINUS_EQUAL,    // -=
	BINARY_OP_DIVIDE_EQUAL,   // /=
	BINARY_OP_MULTIPLY_EQUAL, // *=
	BINARY_OP_MODULUS_EQUAL,  // %=
	BINARY_OP_LESS_THAN,     // <
	BINARY_OP_MORE_THAN,     // >
	BINARY_OP_PLUS,          // +
	BINARY_OP_MINUS,         // -
	BINARY_OP_DIVIDE,        // /
	BINARY_OP_MULTIPLY,      // *
	BINARY_OP_MODULO,        // %
	BINARY_OP_OR,            // divis
	BINARY_OP_AND,           // chord

	// comments
	COMMENT_SINGLE,
	COMMENT_MULTI_START,

	// identifiers
	IDENTIFIER,
	IDENTIFIER_CALL,

	// const values
	CONST_INT,
	CONST_FLOAT,
	CONST_BOOL,
	CONST_CHAR, // '
	CONST_STR, // "

	// unidentified
	ERROR_TOKEN
};
struct Token
{
	CbTokenType type;
	std::optional<std::string> value;

	size_t line;
	size_t column;

	std::filesystem::path path;

	Token();

	Token(const CbTokenType type, const std::optional<std::string> &value, const size_t line, const size_t column, const std::filesystem::path& path);

	bool isConst() const;
};