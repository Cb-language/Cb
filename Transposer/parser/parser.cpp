#include "parser.h"

Token parser::current() const
{
    return tokens[pos];
}

Token parser::advance()
{
    if (!isAtEnd()) return tokens[++pos];
    // TODO: add err after completing the err
    return Token(Token::IDENTIFIER, L"ERR", 0, 0); // meanwhile this is a temp result
}

Token parser::peek() const
{
    if (!isAtEnd()) return tokens[pos+1];
    // TODO: add err after completing the errs
    return Token(Token::IDENTIFIER, L"ERR", 0, 0); // meanwhile this is a temp result
}

Token parser::prev() const
{
    if (pos > 0) return tokens[pos-1];
    // TODO: add err after completing the errs
    return Token(Token::IDENTIFIER, L"ERR", 0, 0); // meanwhile this is a temp result
}

bool parser::isAtEnd() const
{
    return pos >= len;
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

void parser::expect(const Token::TokenType type) const
{
    if (!match(type)) throw std::runtime_error("unexpected token"); // will be replaced by a real err after completing the errs
}

void parser::expect(const Token::TokenType type, const std::wstring& value) const
{
    if (!match(type, value)) throw std::runtime_error("unexpected token"); // will be replaced by a real err after completing the errs
}

parser::parser(const std::vector<Token>& tokens) : tokens(tokens), len(tokens.size()), pos(0)
{
}
