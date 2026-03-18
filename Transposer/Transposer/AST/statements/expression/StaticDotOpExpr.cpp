#include "StaticDotOpExpr.h"

#include <algorithm>

#include "FuncCallExpr.h"
#include "errorHandling/classErrors/IllegalDotOpError.h"
#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "other/SymbolTable.h"

StaticDotOpExpr::StaticDotOpExpr(const Token& token,
                                 std::unique_ptr<ClassType> left, std::unique_ptr<VarReference> right,const bool needsSemicolon)
        : VarReference(token), left(std::move(left)), right(std::move(right))
{
    this->right->setIsClassItem(true);
    StaticDotOpExpr::setNeedsSemicolon(needsSemicolon);
}

std::unique_ptr<IType> StaticDotOpExpr::getType() const
{
    return right->getType();
}

void StaticDotOpExpr::setNeedsSemicolon(const bool needsSemicolon)
{
    Expr::setNeedsSemicolon(needsSemicolon);
    right->setNeedsSemicolon(needsSemicolon);
}

void StaticDotOpExpr::analyze() const
{
    const ClassNode* classNode = SymbolTable::getClass(left->getFQN());
    if (classNode == nullptr)
    {
        symTable->addError(std::make_unique<IllegalDotOpError>(token, left->toString(), right->toString()));
        return;
    }

    if (auto* funcCall = dynamic_cast<FuncCallExpr*>(right.get()))
    {
        funcCall->setTargetClass(classNode);
    }

    right->setSymbolTable(symTable);
    right->setScope(scope);
    right->setClassNode(classNode);
    right->analyze();

    if (const FQN pass = {right->toString()}; !SymbolTable::isLegalFieldOrMethod(symTable, left->copy(), pass, token, currClass))
    {
        symTable->addError(std::make_unique<IllegalDotOpError>(token, left->toString(), right->toString()));
    }
}

std::string StaticDotOpExpr::translateToCpp() const
{
    std::string res = (needsSemicolon && !isClassItem) ? getTabs() : "";
    res += left->toString() + "::" + right->translateToCpp();
    return res;
}

std::string StaticDotOpExpr::toString() const
{
    return left->toString() + "\\" + right->toString();
}

void StaticDotOpExpr::setSymbolTable(SymbolTable* symTable) const
{
    Stmt::setSymbolTable(symTable);
    right->setSymbolTable(symTable);
}
