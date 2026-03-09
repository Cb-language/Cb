#pragma once

#include <vector>
#include <queue>
#include <memory>
#include "errorHandling/Error.h"
#include "AST/abstract/Statement.h"
#include "AST/statements/expression/FuncCallExpr.h"
#include "other/SymbolTable.h"
#include "symbols/FuncCredit.h"
#include "AST/statements/IncludeStmt.h"


class ParserContext
{
private:
    const std::vector<Token> tokens;
    const size_t len;
    size_t pos;
    std::vector<std::unique_ptr<Stmt>> stmts;
    std::queue<FuncCredit> creditsQ;
    std::queue<FuncCallExpr*> callsQ;
    std::vector<std::unique_ptr<IncludeStmt>> includes;
    std::vector<std::unique_ptr<Error>> errors;
    bool hasMain;

public:
    explicit ParserContext(const std::vector<Token>& tokens);


    void addError(std::unique_ptr<Error> err);
    void synchronize();

    const Token& current() const;
    Token advance();
    const Token& peek() const;

    bool isAtEnd() const;

    bool matchNonConsume(const TokenType type) const;
    bool matchConsume(TokenType type);

    bool expect(TokenType type, std::unique_ptr<Error> err = nullptr, std::optional<std::reference_wrapper<Token>> out = std::nullopt);

    bool isAssignmentStmtAhead();
    bool isUnaryOpStmtAhead();
    bool isUnaryOp() const;
    bool isAssignmentOp() const;
    bool isBinaryOp() const;
    bool isType() const;

    bool getHasMain() const;
    const Token& getLast() const;
    const std::vector<std::unique_ptr<Stmt>>& getStmts() const;
    const std::vector<std::unique_ptr<Error>>& getErrors() const;
    const std::vector<Token>& getTokens() const;

    std::vector<std::unique_ptr<Stmt>>& getStmts();
    std::vector<std::unique_ptr<Error>>& getErrors();
    std::vector<std::unique_ptr<IncludeStmt>>& getIncludes();
    size_t getPos() const;
    std::queue<FuncCredit>& getCreditsQ();
    std::queue<FuncCallExpr*>& getCallsQ();
};