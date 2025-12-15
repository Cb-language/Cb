#include "ReturnStmt.h"

ReturnStmt::ReturnStmt(Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<Expr>& rExpr)
: Stmt(scope, funcDecl), rType(rExpr == nullptr ? std::make_unique<Type>(L"fermata") : rExpr->getType()), rExpr(rExpr == nullptr ? nullptr : std::move(rExpr))
{
}

bool ReturnStmt::isLegal() const
{
    if (rExpr == nullptr)
    {
        return rType->getType() == L"fermata";
    }
    return *(rExpr->getType()) == *rType;
}

std::string ReturnStmt::translateToCpp() const
{
    const std::string exprStr = rExpr == nullptr ? "" :  " " + rExpr->translateToCpp();
    return getTabs() + "return" + exprStr +  ";";
}