#pragma once
#include <filesystem>
#include <iostream>
#include <string>

typedef unsigned char byte;

enum class TokenType : byte
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
	KEYWORD_ROOT,           //
	KEYWORD_BASS,           // ctor in initialize line
	KEYWORD_INSTRUMENT,     // class
	KEYWORD_PLAYERSCORE,    // private
	KEYWORD_CONDUCTORSCORE, // public
	KEYWORD_SECTIONSCORE,   // protected
	KEYWORD_TUTTI,          // public
	KEYWORD_SORDINO,        // private
	KEYWORD_SECTION,        // protected
	KEYWORD_PAUSE,          // break
	KEYWORD_RESUME,         // continue
	KEYWORD_BREAK,          //
	KEYWORD_PLAY,           // std::cout
	KEYWORD_PLAYBAR,        // std::cout << std::endl
	KEYWORD_HEAR,           // std::cin
	KEYWORD_A,              // switch
	KEYWORD_B,              // return
	KEYWORD_C,              // case
	KEYWORD_D,              // if
	KEYWORD_E,              // else
	KEYWORD_FMAJ,           // for loop going up
	KEYWORD_FMIN,           // for loop going down
	KEYWORD_G,              // while
	KEYWORD_SONG,           // func
	KEYWORD_FEAT,           // include
	KEYWORD_MOTIF,          // virtual
	KEYWORD_REST,           // pure virtual
	KEYWORD_VARIATION,      // override
	KEYWORD_C_CLEF,         // 𝄡 (dosent compile)

	// punctuation
	PUNCTUATION_ARROW,              // 𝅘𝅥=
	PUNCTUATION_PIPE,               // |
	PUNCTUATION_HASHTAG,            // #
	PUNCTUATION_COMMA,              // ,
	PUNCTUATION_COPYRIGHT,          // ©
	PUNCTUATION_OPEN_SQUARE_BRACE,  // [
	PUNCTUATION_CLOSE_SQUARE_BRACE, // ]
	PUNCTUATION_LOOP_OPEN,          // 𝄕
	PUNCTUATION_LOOP_CLOSE,         // 𝄇
	PUNCTUATION_DOUBLE_LINE,        // 𝄁
	PUNCTUATION_COLON,              // :
	PUNCTUATION_BACKSLASH,          // 𝄍
	PUNCTUATION_DOUBLE_BACKSLASH,   // 𝄓,
	PUNCTUATION_OPEN_PARENTHESIS,   // (
	PUNCTUATION_CLOSE_PARENTHESIS,  // )
	PUNCTUATION_REST,               // 𝄽 (does nothing)
	PUNCTUATION_OPEN_LINE,          // 𝄞
	PUNCTUATION_CLOSE_LINE,         // 𝄀
	PUNCTUATION_CLOSE_FILE,         // 𝄂
	PUNCTUATION_NEW_LINE,           // \n

	// unary operations
	UNARY_OP_SHARP,        // ♯    ++
	UNARY_OP_DOUBLE_SHARP, // 𝄪     ++)++
	UNARY_OP_FLAT,         // ♭     --
	UNARY_OP_DOUBLE_FLAT,  // 𝄫     --)--
	UNARY_OP_NATRUAL,      // ♮      =0
	UNARY_OP_NOT,          // !     !

	// binary operation
	BINARY_OP_EQUIAL,        //==
	BINARY_OP_NOT_EQUAL,     //!=
	BINARY_OP_LESSER_EQUAL,  // <=
	BINARY_OP_GREATER_EQUAL, // >=
	BINARY_OP_LESS_THAN,     // <
	BINARY_OP_MORE_THAN,     // >
	BINARY_OP_PLUS,          // +
	BINARY_OP_MINUS,         // -
	BINARY_OP_DIVIDE,        // /
	BINARY_OP_MULTIPLY,      // *
	BINARY_OP_MODULO,        // %
	BINARY_OP_OR,            // divis
	BINARY_OP_AND,           // chord

	// assignment operations
	ASSIGNMENT_OP_EQUAL,          // =
	ASSIGNMENT_OP_PLUS_EQUAL,     // +=
	ASSIGNMENT_OP_MINUS_EQUAL,    // -=
	ASSIGNMENT_OP_DIVIDE_EQUAL,   // /=
	ASSIGNMENT_OP_MULTIPLY_EQUAL, // *=
	ASSIGNMENT_OP_MODULUS_EQUAL,  // %=

	COMMENT_SINGLE, // ?
	COMMENT_MULTI,  // ?* *?

	UNDEFINED_TOKEN,
	IDENTIFIER,
	IDENTIFIER_CALL,
	CONST_INT,
	CONST_FLOAT,
	CONST_CHAR,
	CONST_BOOL,
	CONST_STR,

};
struct Token
{
	TokenType type;
	std::wstring value;

	size_t line;
	size_t column;

	std::filesystem::path path;

	Token();

	Token(TokenType type, const std::wstring &value, size_t line, size_t column, const std::filesystem::path& path);

	bool isConst() const;
};