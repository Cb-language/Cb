#include "CaseStmt.h"

CaseStmt::CaseStmt(Scope *scope, FuncDeclStmt *funcDecl, std::unique_ptr<Stmt> body, std::unique_ptr<Expr> expr) : Stmt(scope, funcDecl), body(std::move(body)), expr(std::move(expr))
{
}

bool CaseStmt::isLegal() const
{
    return body->isLegal() && expr->isLegal();
}

std::string CaseStmt::translateToCpp() const
{
    std::ostringstream os;
    os << getTabs() << "case " << expr->translateToCpp() << ":\n" << body->translateToCpp();
    return os.str();
}
