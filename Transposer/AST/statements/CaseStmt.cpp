#include "CaseStmt.h"

CaseStmt::CaseStmt(Scope *scope, FuncDeclStmt *funcDecl, std::unique_ptr<Expr> expr, std::unique_ptr<BodyStmt> body,
    const bool isDefault) : Stmt(scope, funcDecl), expr(std::move(expr)), body(std::move(body)), isDefault(isDefault)
{
}

bool CaseStmt::isLegal() const
{
    if (expr == nullptr && isDefault)
    {
        return body->isLegal();
    }
    return body->isLegal() && expr->isLegal() && expr->getType()->isNumberable();
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
    os << ":\n" << body->translateToCpp() << "\n";
    return os.str();
}
