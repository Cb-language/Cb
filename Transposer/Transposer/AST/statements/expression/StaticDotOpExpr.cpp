#include "StaticDotOpExpr.h"

#include <algorithm>

#include "errorHandling/classErrors/IllegalDotOpError.h"
#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "other/SymbolTable.h"

StaticDotOpExpr::StaticDotOpExpr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass,
                                 std::unique_ptr<ClassType> left, std::unique_ptr<Call> right,const bool isStmt)
        : Call(token, scope, funcDecl, currClass), left(std::move(left)), right(std::move(right))
{
    Expr::setIsStmt(isStmt);
}

std::unique_ptr<IType> StaticDotOpExpr::getType() const
{
    return right->getType();
}

void StaticDotOpExpr::analyze() const
{
    if (!SymbolTable::isLegalFieldOrMethod(left->copy(), right->toString(), token, currClass))
    {
        throw IllegalDotOpError(token, left->toString(), Utils::wstrToStr(right->toString()));
    }
}

std::string StaticDotOpExpr::translateToCpp() const
{
    return getTabs() + left->toString() + "::" + right->translateToCpp();
}

std::wstring StaticDotOpExpr::toString() const
{
    return left->getType() + L"\\" + right->toString();
}
