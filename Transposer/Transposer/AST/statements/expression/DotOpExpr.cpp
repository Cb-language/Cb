#include "DotOpExpr.h"

#include <algorithm>

#include "errorHandling/classErrors/IllegalDotOpError.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "other/SymbolTable.h"
#include "symbols/Type/ClassType.h"


DotOpExpr::DotOpExpr(const Token& token,
    std::unique_ptr<Call> left, std::unique_ptr<Call> right) : Call(token), left(std::move(left)), right(std::move(right))
{
}

std::unique_ptr<IType> DotOpExpr::getType() const
{
    return right->getType();
}

void DotOpExpr::analyze() const
{
    const auto leftType = left->getType();

    if (leftType->isPrimitive())
    {
        symTable->addError(std::make_unique<IllegalOpOnType>(token, leftType->toString()));
    }

    const auto rightCasted = dynamic_cast<const Call*>(right.get());

    if (rightCasted == nullptr) symTable->addError(std::make_unique<HowDidYouGetHere>(token));

    if (const FQN passing = {rightCasted->toString()}; !SymbolTable::isLegalFieldOrMethod(leftType, passing, token, currClass))
    {
        symTable->addError(std::make_unique<IllegalDotOpError>(token, left->translateToCpp(), rightCasted->toString()));
    }
}

std::string DotOpExpr::translateToCpp() const
{
    return getTabs() + left->translateToCpp() + "->" + right->translateToCpp();
}

std::string DotOpExpr::toString() const
{
    return left->toString() + "\\" + right->toString();
}
