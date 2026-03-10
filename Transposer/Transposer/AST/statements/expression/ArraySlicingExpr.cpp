#include "ArraySlicingExpr.h"

#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"

ArraySlicingExpr::ArraySlicingExpr(const Token& token, IFuncDeclStmt* funcDecl, std::unique_ptr<Call> call,
    ClassDeclStmt* classDecl)
    : Call(token, funcDecl, classDecl), call(std::move(call))
{
}

ArraySlicingExpr::ArraySlicingExpr(const ArraySlicingExpr& other)
    : Call(other.token, other.funcDecl, other.classDecl), call(other.call.get()), start(other.start.get()), stop(other.stop.get()), step(other.step.get())
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

std::string ArraySlicingExpr::toString() const
{
    return call->toString();
}

void ArraySlicingExpr::setStart(std::unique_ptr<Expr> start)
{
    this->start = std::move(start);
}

void ArraySlicingExpr::setStop(std::unique_ptr<Expr> stop)
{
    this->stop = std::move(stop);
}

void ArraySlicingExpr::setStep(std::unique_ptr<Expr> step)
{
    this->step = std::move(step);
}
