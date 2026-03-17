#include "ArraySlicingExpr.h"

#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"
#include "other/SymbolTable.h"

ArraySlicingExpr::ArraySlicingExpr(const Token& token, std::unique_ptr<VarReference> ref) : VarReference(token), varRef(std::move(ref))
{
}

ArraySlicingExpr::ArraySlicingExpr(const ArraySlicingExpr& other)
    : VarReference(other.token), varRef(other.varRef.get()), start(other.start.get()), stop(other.stop.get()), step(other.step.get())
{
}

void ArraySlicingExpr::analyze() const
{
    if (symTable == nullptr) return;

    varRef->setSymbolTable(symTable);
    varRef->setScope(scope);
    varRef->setClassNode(currClass);
    varRef->analyze();

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

    if (varRef->getType()->getArrLevel() == 0)
    {
        symTable->addError(std::make_unique<IllegalOpOnType>(token, varRef->getType()->toString()));
    }

    if (!start->getType()->isNumberable())
    {
        symTable->addError(std::make_unique<IllegalTypeCast>(token, start->getType()->toString(), "degree"));
    }

    if (!step->getType()->isNumberable())
    {
        symTable->addError(std::make_unique<IllegalTypeCast>(token, step->getType()->toString(), "degree"));
    }

    if (!stop->getType()->isNumberable())
    {
        symTable->addError(std::make_unique<IllegalTypeCast>(token, stop->getType()->toString(), "degree"));
    }
}

std::string ArraySlicingExpr::translateToCpp() const
{
    std::ostringstream oss;
    if (needsSemicolon) oss << getTabs();
    oss << varRef->translateToCpp() << ".slice(" << start->translateToCpp() << ", " << stop->translateToCpp() << ", " << step->translateToCpp() << ")";
    if (needsSemicolon) oss << ";";
    return oss.str();
}

std::unique_ptr<IType> ArraySlicingExpr::getType() const
{
    const auto t = varRef->getType();
    return t ? t->copy() : nullptr;
}

std::string ArraySlicingExpr::toString() const
{
    return varRef->toString();
}

void ArraySlicingExpr::setSymbolTable(SymbolTable* symTable) const
{
    Stmt::setSymbolTable(symTable);
    varRef->setSymbolTable(symTable);
    if (start != nullptr) start->setSymbolTable(symTable);
    if (stop != nullptr) stop->setSymbolTable(symTable);
    if (step != nullptr) step->setSymbolTable(symTable);
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
