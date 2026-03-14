#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <stack>

#include "errorHandling/Error.h"
#include "AST/abstract/Statement.h"
#include "AST/statements/expression/FuncCallExpr.h"
#include "other/SymbolTable.h"
#include "AST/statements/IncludeStmt.h"
#include "FQN.h"

class ParserContext
{
private:
    std::queue<Token> tokens;
    const size_t len;
    std::vector<std::unique_ptr<Stmt>> stmts;
    std::vector<std::unique_ptr<IncludeStmt>> includes;
    std::vector<std::unique_ptr<Error>> errors;

    Token firstToken;
    Token lastToken;

    int breakables;
    int continueables;

    bool isNewLine;
    bool isInFunc;
    void eatRest();
    void eatFuncNewLine();
public:
    explicit ParserContext(const std::queue<Token>& tokens);


    void addError(std::unique_ptr<Error> err);

    const Token& current();
    Token getLastToken();
    Token copyCurrent();

    Token advance();
    void expectSemiColon();

    bool matchConsume(const CbTokenType type, const std::optional<std::reference_wrapper<Token>> out = std::nullopt);
    bool matchNonConsume(CbTokenType type);

    bool expect(CbTokenType type, std::unique_ptr<Error> err = nullptr, std::optional<std::reference_wrapper<Token>> out = std::nullopt);

    FQN parseFQN();

    bool isUnaryOp();
    bool isBinaryOp();
    bool isType() const;

    const std::vector<std::unique_ptr<Stmt>>& getStmts() const;
    const std::vector<std::unique_ptr<Error>>& getErrors() const;

    std::vector<std::unique_ptr<Stmt>>& getStmts();
    std::vector<std::unique_ptr<Error>>& getErrors();
    std::vector<std::unique_ptr<IncludeStmt>>& getIncludes();

    void setIsInFunc(const bool isInFunc);
    bool getIsInFunc() const;

    void addBreakable();
    void removeBreakable();

    void addContinueable();
    void removeContinueable();

    bool getIsBreakable() const;
    bool getIsContinueable() const;

    bool isEmpty() const;

    const Token& getFirstToken() const;
};