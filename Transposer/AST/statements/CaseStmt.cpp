#include "CaseStmt.h"

CaseStmt::CaseStmt(Scope *scope, FuncDeclStmt *funcDecl, std::unique_ptr<Expr> expr, std::unique_ptr<Stmt> body,
    const bool isDefault) : Stmt(scope, funcDecl), expr(std::move(expr)), body(std::move(body)), isDefault(isDefault)
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
