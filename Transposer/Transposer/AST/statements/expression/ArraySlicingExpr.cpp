#include "ArraySlicingExpr.h"

#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"

ArraySlicingExpr::ArraySlicingExpr(const Token& token, std::unique_ptr<Call> call) : Call(token), call(std::move(call))
{
}

ArraySlicingExpr::ArraySlicingExpr(const ArraySlicingExpr& other)
    : Call(other.token), call(other.call.get()), start(other.start.get()), stop(other.stop.get()), step(other.step.get())
{
}

void ArraySlicingExpr::analyze() const
{
    if (symTable == nullptr) return;

    call->setSymbolTable(symTable);
    call->setScope(scope);
    call->setClassNode(currClass);
    call->analyze();

    start->setSymbolTable(symTable);
    start->setScope(scope);
    start->setClassNode(currClass);
    start->analyze();

    stop->setSymbolTable(symTable);
    stop->setScope(scope);
    stop->setClassNode(currClass);
    stop->analyze();

    step->setSymbolTable(symTable);
    step->setScope(scope);
    step->setClassNode(currClass);
    step->analyze();

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
}

std::string ArraySlicingExpr::translateToCpp() const
{
    std::ostringstream oss;
    if (needsSemicolon) oss << getTabs();
    oss << call->translateToCpp() << "[" << start->translateToCpp() << " : " << stop->translateToCpp() << " : " << step->translateToCpp() << "]";
    if (needsSemicolon) oss << ";";
    return oss.str();
}

std::unique_ptr<IType> ArraySlicingExpr::getType() const
{
    const auto t = call->getType();
    return t ? t->copy() : nullptr;
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
