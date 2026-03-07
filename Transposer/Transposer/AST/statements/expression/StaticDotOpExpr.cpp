#include "StaticDotOpExpr.h"

#include <algorithm>

#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "other/SymbolTable.h"

StaticDotOpExpr::StaticDotOpExpr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass,
                                 std::unique_ptr<ClassType> left, std::unique_ptr<Call> right) : Expr(token, scope, funcDecl, currClass), left(std::move(left)), right(std::move(right))
{
}

std::unique_ptr<IType> StaticDotOpExpr::getType() const
{
    return right->getType();
}

void StaticDotOpExpr::analyze() const
{
    if (!SymbolTable::isLegalFieldOrMethod(left->copy(), right->toString(), token, left->getClass()))
    {
        throw IllegalOpOnType(token, left->toString());
    }
}

std::string StaticDotOpExpr::translateToCpp() const
{
    return getTabs() + left->toString() + "::" + right->translateToCpp();
}
