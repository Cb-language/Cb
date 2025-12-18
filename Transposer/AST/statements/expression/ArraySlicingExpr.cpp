#include "ArraySlicingExpr.h"

ArraySlicingExpr::ArraySlicingExpr(Scope* scope, FuncDeclStmt* funcDecl, const Var& var, const int start, const int stop, const int step)
    : VarCallExpr(scope, funcDecl, var), start(start), stop(stop), step(step)
{
}

ArraySlicingExpr::ArraySlicingExpr(const ArraySlicingExpr& other)
    : VarCallExpr(other.scope, other.funcDecl, other.var.copy()), start(other.start), stop(other.stop), step(other.step)
{
}

bool ArraySlicingExpr::isLegal() const
{
    return var.getType()->getArrLevel() > 0;
}

std::string ArraySlicingExpr::translateToCpp() const
{
    std::ostringstream oss;

    oss << Utils::wstrToStr(var.getName()) << ".slice(" << start << ", " << stop << ", " << step << ")";
    return oss.str();
}

std::unique_ptr<IType> ArraySlicingExpr::getType() const
{
    return var.getType()->copy();
}
