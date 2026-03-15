#include "ArrayIndexingExpr.h"

#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"
#include "other/SymbolTable.h"

ArrayIndexingExpr::ArrayIndexingExpr(const Token& token, std::unique_ptr<VarReference> ref, std::unique_ptr<Expr> index)
    : VarReference(token), varRef(std::move(ref)) ,index(std::move(index))
{
}

std::unique_ptr<IType> ArrayIndexingExpr::getType() const
{
    const auto t = varRef->getType();
    if (!t) return nullptr;
    const auto arrT = t->getArrType();
    return arrT ? arrT->copy() : nullptr;
}

std::string ArrayIndexingExpr::translateToCpp() const
{
    std::string res = needsSemicolon ? getTabs() : "";
    res += varRef->translateToCpp() + "[" + index->translateToCpp() + "]";
    if (needsSemicolon) res += ";";
    return res;
}

void ArrayIndexingExpr::analyze() const
{
    if (symTable == nullptr) return;

    varRef->setSymbolTable(symTable);
    varRef->setScope(scope);
    varRef->setClassNode(currClass);
    varRef->analyze();

    index->setSymbolTable(symTable);
    index->setScope(scope);
    index->setClassNode(currClass);
    index->analyze();

    if (varRef->getType()->getArrLevel() == 0)
    {
        symTable->addError(std::make_unique<IllegalOpOnType>(token, varRef->getType()->toString()));
    }

    if (!index->getType()->isNumberable())
    {
        symTable->addError(std::make_unique<IllegalTypeCast>(token, index->getType()->toString(), "degree"));
    }
}

std::string ArrayIndexingExpr::toString() const
{
    return varRef->toString();
}

void ArrayIndexingExpr::setSymbolTable(SymbolTable* symTable) const
{
    Stmt::setSymbolTable(symTable);
    varRef->setSymbolTable(symTable);
    index->setSymbolTable(symTable);
}
