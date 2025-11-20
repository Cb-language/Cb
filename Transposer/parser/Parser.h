#pragma once
#include <vector>

#include "../AST/Statements.h"
#include "errorHandling/Error.h"
#include "other/SymbolTable.h"
#include "token/Token.h"

class Parser
{
private:
    const std::vector<Token> tokens;
    const size_t len;
    size_t pos;

    SymbolTable symTable;

    const Token& current() const;
    const Token& advance();
    const Token& peek() const;
    const Token& prev() const;
    bool isAtEnd() const;

    bool match(const Token::TokenType type) const;
    bool match(const Token::TokenType type, const std::wstring& value) const;

    void expect(const Token::TokenType type);
    void expect(const Token::TokenType type, const std::wstring& value);

    void expect(const Token::TokenType type, const Error& err);
    void expect(const Token::TokenType type, const std::wstring& value, const Error& err);

    const Token& expectAndGet(const Token::TokenType type);
    const Token& expectAndGet(const Token::TokenType type, const std::wstring& value);

    const Token& expectAndGet(const Token::TokenType type, const Error& err);
    const Token& expectAndGet(const Token::TokenType type, const std::wstring& value, const Error& err);


    // General Statements

    std::unique_ptr<VarDecStmt> parseVarDecStmt();
    std::unique_ptr<AssignmentStmt> parseAssignmentStmt();

    // Expressions

    std::unique_ptr<Expr> parseExpr();
    std::unique_ptr<ConstValueExpr> parseConstValue();
    std::unique_ptr<VarCallExpr> parseVarCallExpr();

public:
    explicit Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();
};

