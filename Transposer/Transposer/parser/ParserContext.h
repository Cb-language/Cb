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
    SymbolTable symTable;

public:
    explicit ParserContext(const std::vector<Token>& tokens);


    // void addError(Error* err);
    // void synchronize();
    //
    // const Token& current() const;
    // Token advance();
    // const Token& peek() const;
    // const Token& prev() const;
    // bool isAtEnd() const;
    //
    // bool match(TokenType type) const;
    // bool match(TokenType type, const std::string& value) const;
    //
    // bool expect(TokenType type);
    // bool expect(TokenType type, const std::string& value);
    //
    // bool expect(TokenType type, Error* err);
    // bool expect(TokenType type, const std::string& value, Error* err);
    //
    // bool expectAndGet(TokenType type, Token& out);
    // bool expectAndGet(TokenType type, const std::string& value, Token& out);
    //
    // bool expectAndGet(TokenType type, Error* err, Token& out);
    // bool expectAndGet(TokenType type, const std::string& value, Error* err, Token& out);
    //
    // bool isAssignmentStmtAhead();
    // bool isUnaryOpStmtAhead();


    void addToSymTable(const SymbolTable& symTable);
    const SymbolTable& getSymTable() const;
    bool getHasMain() const;
    const Token& getLast() const;
    const std::vector<std::unique_ptr<Stmt>>& getStmts() const;
    const std::vector<std::unique_ptr<Error>>& getErrors() const;

    SymbolTable& getSymTable();
    std::vector<std::unique_ptr<Stmt>>& getStmts();
    std::vector<std::unique_ptr<Error>>& getErrors();

};