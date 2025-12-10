#pragma once
#include <vector>
#include <queue>

#include "AST/abstract/Statement.h"
#include "AST/statements/AssignmentStmt.h"
#include "AST/statements/BodyStmt.h"
#include "AST/statements/FuncCreditStmt.h"
#include "AST/statements/HearStmt.h"
#include "AST/statements/PlayStmt.h"
#include "AST/statements/VarDecStmt.h"
#include "AST/statements/expression/UnaryOpExpr.h"
#include "AST/statements/ReturnStmt.h"
#include "symbols/FuncCredit.h"

class Error;

class Parser
{
private:
    const std::vector<Token> tokens;
    const size_t len;
    size_t pos;
    std::vector<std::unique_ptr<Stmt>> stmts;
    std::queue<FuncCredit> creditsQ;
    std::queue<FuncCallExpr*> callsQ;

    bool hasMain;

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
    std::unique_ptr<PlayStmt> parsePlayStmt();
    std::unique_ptr<BodyStmt> parseBodyStmt(const bool isGlobal = false);
    std::unique_ptr<FuncDeclStmt> parseFuncDeclStmt();
    std::unique_ptr<ReturnStmt> parseReturnStmt();
    std::unique_ptr<FuncCreditStmt> parseFuncCreditStmt();

    // Expressions

    std::unique_ptr<Expr> parseExpr(const bool hasParens = false);
    std::unique_ptr<Expr> parsePrimary();
    std::unique_ptr<Expr> parseBinaryOpRight(int exprPrec,  std::unique_ptr<Expr> left);
    std::unique_ptr<ConstValueExpr> parseConstValueExpr();
    std::unique_ptr<VarCallExpr> parseVarCallExpr();
    std::unique_ptr<UnaryOpExpr> parseUnaryOpExpr(const bool isStmt = false);
    std::unique_ptr<FuncCallExpr> parseFuncCallExpr(const bool isStmt = false);

public:
    explicit Parser(const std::vector<Token>& tokens);
    ~Parser();
    void parse();
    bool checkLegal();
    std::string translateToCpp() const;
};

