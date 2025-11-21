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
    std::vector<std::unique_ptr<Stmt>> stmts;

    SymbolTable symTable;

    const Token& current() const;
    const Token& advance();
    const Token& peek() const;
    const Token& prev() const;
    bool isAtEnd() const;

    bool match(Token::TokenType type) const;
    bool match(Token::TokenType type, const std::wstring& value) const;

    void expect(Token::TokenType type);
    void expect(Token::TokenType type, const std::wstring& value);

    void expect(Token::TokenType type, const Error& err);
    void expect(Token::TokenType type, const std::wstring& value, const Error& err);

    const Token& expectAndGet(Token::TokenType type);
    const Token& expectAndGet(Token::TokenType type, const std::wstring& value);

    const Token& expectAndGet(Token::TokenType type, const Error& err);
    const Token& expectAndGet(Token::TokenType type, const std::wstring& value, const Error& err);


    // General Statements

    std::unique_ptr<VarDecStmt> parseVarDecStmt();
    std::unique_ptr<AssignmentStmt> parseAssignmentStmt();
    std::unique_ptr<HearStmt> parseHearStmt();

    // Expressions

    std::unique_ptr<Expr> parseExpr(const bool hasParens = false);
    std::unique_ptr<Expr> parsePrimary();
    std::unique_ptr<Expr> parseBinaryOpRight(int exprPrec,  std::unique_ptr<Expr> left);
    std::unique_ptr<ConstValueExpr> parseConstValueExpr();
    std::unique_ptr<VarCallExpr> parseVarCallExpr();

public:
    explicit Parser(const std::vector<Token>& tokens);
    ~Parser();
    void parse();
    bool checkLegal() const;
    std::string translateToCpp() const;
};

