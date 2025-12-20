#include "ArraySlicingExpr.h"

ArraySlicingExpr::ArraySlicingExpr(Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<Call> call, std::unique_ptr<Expr> start, std::unique_ptr<Expr> stop, std::unique_ptr<Expr> step)
    : Call(scope, funcDecl), call(std::move(call)), start(std::move(start)), stop(std::move(stop)), step(std::move(step))
{
}

ArraySlicingExpr::ArraySlicingExpr(const ArraySlicingExpr& other)
    : Call(other.scope, other.funcDecl), call(other.call.get()), start(other.start.get()), stop(other.stop.get()), step(other.step.get())
{
}

bool ArraySlicingExpr::isLegal() const
{
    return call->getType()->getArrLevel() > 0 && call->isLegal() && start->isLegal() && stop->isLegal() && step->isLegal()
    && start->getType()->isNumberable() && stop->getType()->isNumberable() && step->getType()->isNumberable();
}

std::string ArraySlicingExpr::translateToCpp() const
{
    std::ostringstream oss;

    oss << call->translateToCpp() << ".slice(" << start->translateToCpp() << ", " << stop->translateToCpp() << ", " << step->translateToCpp() << ")";
    return oss.str();
}

std::unique_ptr<IType> ArraySlicingExpr::getType() const
{
    return call->getType()->copy();
}
