#include "parser.h"

#include "errorHandling/lexicalErrors/UnexpectedEOF.h"
#include "errorHandling/syntaxErrors/UnexpectedToken.h"

parser::parser(const std::vector<Token>& tokens) : tokens(tokens), len(tokens.size()), pos(0), symTable(SymbolTable())
{
}

Token parser::current() const
{
    return tokens[pos];
}

Token parser::advance()
{
    if (isAtEnd())
    {
        throw UnexpectedEOF(tokens.back());
    }
    return tokens[++pos];
}

Token parser::peek() const
{
    if (isAtEnd())
    {
        throw UnexpectedEOF(tokens.back());
    }

    return tokens[pos + 1];
}

Token parser::prev() const
{
    if (pos <= 0)
    {
        throw UnexpectedEOF(tokens.front());
    }
    return tokens[pos - 1];
}

bool parser::isAtEnd() const
{
    return pos + 1 >= len;
}

bool parser::match(const Token::TokenType type) const
{
    return current().type == type;
}

bool parser::match(const Token::TokenType type, const std::wstring& value) const
{
    const Token t = current();

    return t.type == type && t.value == value;
}

void parser::expect(const Token::TokenType type)
{
    if (!match(type))
    {
        throw UnexpectedToken(current());
    }
    advance();
}

void parser::expect(const Token::TokenType type, const std::wstring& value)
{
    if (!match(type, value))
    {
        throw UnexpectedToken(current());
    }
    advance();
}
