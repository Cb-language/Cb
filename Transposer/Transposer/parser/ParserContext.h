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

enum class SemiColonEatType
{
    IN_SCOPE,
    OUT_SCOPE
};


class ParserContext
{
private:
    std::queue<Token> tokens;
    const size_t len;
    std::vector<std::unique_ptr<Stmt>> stmts;
    std::vector<std::unique_ptr<IncludeStmt>> includes;
    std::vector<std::unique_ptr<Error>> errors;

    IFuncDeclStmt* funcDecl;
    std::stack<std::reference_wrapper<ClassDeclStmt>> classDecl;

    bool hasMain;
    Token firstToken;


    bool isNewLine;
    bool isInFunc;
    void eatRest();
    void eatFuncNewLine();
public:
    explicit ParserContext(const std::queue<Token>& tokens);


    void addError(std::unique_ptr<Error> err);

    const Token& current() const;
    Token copyCurrent();

    Token advance();
    SemiColonEatType expectSemiColon(const bool isInFunc);

    bool matchConsume(const CbTokenType type, const std::optional<std::reference_wrapper<Token>> out = std::nullopt);
    bool matchNonConsume(CbTokenType type) const;

    bool expect(CbTokenType type, std::unique_ptr<Error> err = nullptr, std::optional<std::reference_wrapper<Token>> out = std::nullopt);

    FQN parseFQN();

    bool isUnaryOp() const;
    bool isBinaryOp() const;
    bool isType() const;

    bool getHasMain() const;
    const std::vector<std::unique_ptr<Stmt>>& getStmts() const;
    const std::vector<std::unique_ptr<Error>>& getErrors() const;

    std::vector<std::unique_ptr<Stmt>>& getStmts();
    std::vector<std::unique_ptr<Error>>& getErrors();
    std::vector<std::unique_ptr<IncludeStmt>>& getIncludes();

    IFuncDeclStmt* getFuncDecl() const;
    ClassDeclStmt* getClassDecl() const;
    void pushClassDecl(ClassDeclStmt& decl);
    void popClassDecl();
    void setFuncDecl(IFuncDeclStmt* funcDecl);
    void setIsInFunc(const bool isInFunc);


    bool isEmpty() const;

    const Token& getFirstToken() const;
};