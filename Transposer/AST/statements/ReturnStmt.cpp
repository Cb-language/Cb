#include "ReturnStmt.h"

ReturnStmt::ReturnStmt(Scope* scope, FuncDeclStmt* funcDecl, const Type& rType, const Expr* rExpr) : Stmt(scope, funcDecl), rType(rType), rExpr(rExpr)
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
