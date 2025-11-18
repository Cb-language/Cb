#pragma once
#include <vector>

#include "other/SymbolTable.h"
#include "token/Token.h"

class parser
{
private:
    const std::vector<Token> tokens;
    const size_t len;
    size_t pos;

    SymbolTable symTable;

    Token current() const;
    Token advance();
    Token peek() const;
    Token prev() const;
    bool isAtEnd() const;

    bool match(const Token::TokenType type) const;
    bool match(const Token::TokenType type, const std::wstring& value) const;

    void expect(const Token::TokenType type) const;
    void expect(const Token::TokenType type, const std::wstring& value) const;


public:
    explicit parser(const std::vector<Token>& tokens);
};

