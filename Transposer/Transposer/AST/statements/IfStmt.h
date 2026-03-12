#pragma once
#include "BodyStmt.h"
#include "AST/abstract/Expression.h"
#include "AST/abstract/Statement.h"
#include <sstream>

struct StmtWithBody
{
    std::unique_ptr<Expr> expr;
    std::unique_ptr<Stmt> body;

    StmtWithBody(std::unique_ptr<Expr> expr, std::unique_ptr<Stmt> body) : expr(std::move(expr)), body(std::move(body)) {}
};

class IfStmt : public Stmt
{
private:
    StmtWithBody ifStmt;
    std::vector<StmtWithBody> elseStmts;
public:
    IfStmt(const Token& token,
        StmtWithBody ifStmt, std::vector<StmtWithBody>& elseStmts);

    void analyze() const override;
    std::string translateToCpp() const override;
};