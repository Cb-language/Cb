#include "StaticDotOpExpr.h"

#include <algorithm>

#include "errorHandling/classErrors/IllegalDotOpError.h"
#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "other/SymbolTable.h"

StaticDotOpExpr::StaticDotOpExpr(const Token& token,
                                 std::unique_ptr<ClassType> left, std::unique_ptr<Call> right,const bool needsSemicolon)
        : Call(token), left(std::move(left)), right(std::move(right))
{
    Expr::setNeedsSemicolon(needsSemicolon);
}

std::unique_ptr<IType> StaticDotOpExpr::getType() const
{
    return right->getType();
}

void StaticDotOpExpr::analyze() const
{
    if (const FQN pass = {right->toString()}; !SymbolTable::isLegalFieldOrMethod(symTable, left->copy(), pass, token, currClass))
    {
        symTable->addError(std::make_unique<IllegalDotOpError>(token, left->toString(), right->toString()));
    }
}

std::string StaticDotOpExpr::translateToCpp() const
{
    return getTabs() + left->toString() + "::" + right->translateToCpp();
}

std::string StaticDotOpExpr::toString() const
{
    return left->toString() + "\\" + right->toString();
}
