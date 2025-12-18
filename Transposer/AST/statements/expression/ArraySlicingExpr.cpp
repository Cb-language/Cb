#include "ArraySlicingExpr.h"

ArraySlicingExpr::ArraySlicingExpr(Scope* scope, FuncDeclStmt* funcDecl, const Var& var, std::unique_ptr<Expr> start, std::unique_ptr<Expr> stop, std::unique_ptr<Expr> step)
    : VarCallExpr(scope, funcDecl, var), start(std::move(start)), stop(std::move(stop)), step(std::move(step))
{
}

ArraySlicingExpr::ArraySlicingExpr(const ArraySlicingExpr& other)
    : VarCallExpr(other.scope, other.funcDecl, other.var.copy()), start(other.start.get()), stop(other.stop.get()), step(other.step.get())
{
}

bool ArraySlicingExpr::isLegal() const
{
    return var.getType()->getArrLevel() > 0 && start->isLegal() && stop->isLegal() && step->isLegal()
    && start->getType()->isNumberable() && stop->getType()->isNumberable() && step->getType()->isNumberable();
}

std::string ArraySlicingExpr::translateToCpp() const
{
    std::ostringstream oss;

    oss << Utils::wstrToStr(var.getName()) << ".slice(" << start->translateToCpp() << ", " << stop->translateToCpp() << ", " << step->translateToCpp() << ")";
    return oss.str();
}

std::unique_ptr<IType> ArraySlicingExpr::getType() const
{
    return var.getType()->copy();
}
