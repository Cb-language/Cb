#include "CaseStmt.h"

CaseStmt::CaseStmt(Scope *scope, FuncDeclStmt *funcDecl, std::unique_ptr<Stmt> body, std::unique_ptr<Expr> expr,
    const bool isDefault) : Stmt(scope, funcDecl), body(std::move(body)), expr(std::move(expr)), isDefault(isDefault)
{
}

bool CaseStmt::isLegal() const
{
    return body->isLegal() && expr->isLegal();
}

std::string CaseStmt::translateToCpp() const
{
    std::ostringstream os;
    os << getTabs();

    if (isDefault)
    {
        os << "default";
    }
    else
    {
        os << "case " << expr->translateToCpp();
    }
    os << ":\n" << body->translateToCpp();
    return os.str();
}
