#include "ArraySlicingExpr.h"

#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"

ArraySlicingExpr::ArraySlicingExpr(const Token& token, Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<Call> call, std::unique_ptr<Expr> start, std::unique_ptr<Expr> stop, std::unique_ptr<Expr> step)
    : Call(token, scope, funcDecl), call(std::move(call)), start(std::move(start)), stop(std::move(stop)), step(std::move(step))
{
}

ArraySlicingExpr::ArraySlicingExpr(const ArraySlicingExpr& other)
    : Call(other.token, other.scope, other.funcDecl), call(other.call.get()), start(other.start.get()), stop(other.stop.get()), step(other.step.get())
{
}

void ArraySlicingExpr::analyze() const
{
    if (call->getType()->getArrLevel() == 0)
    {
        throw IllegalOpOnType(token, call->getType()->toString());
    }

    if (!start->getType()->isNumberable())
    {
        throw IllegalTypeCast(token, start->getType()->toString(), "degree");
    }

    if (!step->getType()->isNumberable())
    {
        throw IllegalTypeCast(token, step->getType()->toString(), "degree");
    }

    if (!stop->getType()->isNumberable())
    {
        throw IllegalTypeCast(token, stop->getType()->toString(), "degree");
    }

    call->analyze();
    start->analyze();
    stop->analyze();
    step->analyze();
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
