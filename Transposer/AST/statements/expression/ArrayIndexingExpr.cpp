#include "ArrayIndexingExpr.h"

ArrayIndexingExpr::ArrayIndexingExpr(Scope* scope, FuncDeclStmt* funcDecl, const Var& var, std::unique_ptr<Expr> index) : VarCallExpr(scope, funcDecl, var), index(std::move(index))
{
}

std::unique_ptr<IType> ArrayIndexingExpr::getType() const
{
    return var.getType()->getArrType()->copy();
}

std::string ArrayIndexingExpr::translateToCpp() const
{
    return Utils::wstrToStr(var.getName()) + "[" + index->translateToCpp() + "]";
}

bool ArrayIndexingExpr::isLegal() const
{
    return var.getType()->getArrLevel() > 0 && index->isLegal() && index->getType()->isNumberable();
}
