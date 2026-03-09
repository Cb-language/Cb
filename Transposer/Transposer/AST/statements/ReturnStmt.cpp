#include "ReturnStmt.h"

#include "errorHandling/semanticErrors/WrongReturnType.h"

ReturnStmt::ReturnStmt(const Token& token, IFuncDeclStmt* funcDecl, std::unique_ptr<Expr>& rExpr, std::unique_ptr<IType> rType, ClassDeclStmt* classDecl)
        : Stmt(token, funcDecl, classDecl), rType(std::move(rType)),
        rExpr(rExpr == nullptr ? nullptr : std::move(rExpr))
{
}

void ReturnStmt::analyze() const
{
    if (rExpr == nullptr && rType->getType() != "fermata")
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