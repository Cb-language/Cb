#include "ReturnStmt.h"

#include "errorHandling/semanticErrors/WrongReturnType.h"

ReturnStmt::ReturnStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass, std::unique_ptr<Expr>& rExpr, std::unique_ptr<IType> rType)
        : Stmt(token, scope, funcDecl, currClass), rType(std::move(rType)),
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