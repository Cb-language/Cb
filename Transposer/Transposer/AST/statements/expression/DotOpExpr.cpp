#include "DotOpExpr.h"

#include <algorithm>

#include "errorHandling/classErrors/IllegalDotOpError.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "other/SymbolTable.h"
#include "symbols/Type/ClassType.h"

DotOpExpr::DotOpExpr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass,
                     const std::wstring& op, std::unique_ptr<Call> left, std::unique_ptr<Call> right, const bool hasParens)
        : BinaryOpExpr(token, scope, funcDecl, currClass, op, std::move(left), std::move(right), hasParens)
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
        throw IllegalOpOnType(token, leftType->toString());
    }

    const auto rightCasted = dynamic_cast<const Call*>(right.get());

    if (rightCasted == nullptr) throw HowDidYouGetHere(token);

    if (!SymbolTable::isLegalFieldOrMethod(leftType, rightCasted->toString(), token, currClass))
    {
        throw IllegalDotOpError(token, left->translateToCpp(), Utils::wstrToStr(rightCasted->toString()), leftType->toString());
    }
}

std::string DotOpExpr::translateToCpp() const
{
    return getTabs() + left->translateToCpp() + "->" + right->translateToCpp();
}
