#include "DotOpExpr.h"

#include <algorithm>

#include "FuncCallExpr.h"
#include "errorHandling/classErrors/IllegalDotOpError.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "other/SymbolTable.h"
#include "symbols/Type/ClassType.h"


DotOpExpr::DotOpExpr(const Token& token,
    std::unique_ptr<VarReference> left, std::unique_ptr<VarReference> right) : VarReference(token), left(std::move(left)), right(std::move(right))
{
    this->right->setIsClassItem(true);
}

std::unique_ptr<IType> DotOpExpr::getType() const
{
    return right->getType();
}

void DotOpExpr::setNeedsSemicolon(const bool needsSemicolon)
{
    Expr::setNeedsSemicolon(needsSemicolon);
    right->setNeedsSemicolon(needsSemicolon);
}

void DotOpExpr::analyze() const
{
    left->setSymbolTable(symTable);
    left->setScope(scope);
    left->setClassNode(currClass);
    left->analyze();

    const auto leftType = left->getType();

    if (leftType == nullptr || leftType->isPrimitive())
    {
        symTable->addError(std::make_unique<IllegalOpOnType>(token, leftType ? leftType->toString() : "fermata"));
        return;
    }

    const auto rightCasted = right.get();

    if (rightCasted == nullptr) symTable->addError(std::make_unique<HowDidYouGetHere>(token));

    const ClassNode* classNode = SymbolTable::getClass(leftType->getFQN());
    if (classNode == nullptr)
    {
        symTable->addError(std::make_unique<IllegalDotOpError>(token, left->translateToCpp(), rightCasted->toString()));
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

    if (const FQN passing = {rightCasted->toString()}; !SymbolTable::isLegalFieldOrMethod(symTable, leftType, passing, token, currClass))
    {
        symTable->addError(std::make_unique<IllegalDotOpError>(token, left->translateToCpp(), rightCasted->toString()));
    }
}

std::string DotOpExpr::translateToCpp() const
{
    std::string res = (needsSemicolon && !isClassItem) ? getTabs() : "";
    res += left->translateToCpp() + "->" + right->translateToCpp();
    return res;
}

std::string DotOpExpr::toString() const
{
    return left->toString() + "\\" + right->toString();
}

void DotOpExpr::setSymbolTable(SymbolTable* symTable) const
{
    Stmt::setSymbolTable(symTable);
    left->setSymbolTable(symTable);
    right->setSymbolTable(symTable);
}
