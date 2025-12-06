#include "ReturnStmt.h"

ReturnStmt::ReturnStmt(Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<Expr>& rExpr) : Stmt(scope, funcDecl), rType(rExpr->getType()), rExpr(std::move(rExpr))
{
}

bool ReturnStmt::isLegal() const
{
    return rExpr->getType() == rType;
}

std::string ReturnStmt::translateToCpp() const
{
    return getTabs() + "return " + rExpr->translateToCpp() + ";";
}
