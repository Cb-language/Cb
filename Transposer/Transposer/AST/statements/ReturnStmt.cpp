#include "ReturnStmt.h"

#include "errorHandling/semanticErrors/WrongReturnType.h"

ReturnStmt::ReturnStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, ClassNode* currClass, std::unique_ptr<Expr>& rExpr)
        : Stmt(token, scope, funcDecl, currClass), rType(rExpr == nullptr ? std::make_unique<Type>(L"fermata") : rExpr->getType()),
        rExpr(rExpr == nullptr ? nullptr : std::move(rExpr))
{
}

void ReturnStmt::analyze() const
{
    if (rExpr == nullptr && rType->getType() != L"fermata")
    {
        throw WrongReturnType(token);
    }
    if (rExpr != nullptr && *(rExpr->getType()) != *rType)
    {
        throw WrongReturnType(token);
    }
}

std::string ReturnStmt::translateToCpp() const
{
    const std::string exprStr = rExpr == nullptr ? "" :  " " + rExpr->translateToCpp();
    return getTabs() + "return" + exprStr +  ";";
}