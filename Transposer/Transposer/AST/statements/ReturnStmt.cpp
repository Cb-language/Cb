#include "ReturnStmt.h"
#include "other/SymbolTable.h"
#include "errorHandling/semanticErrors/WrongReturnType.h"

ReturnStmt::ReturnStmt(const Token& token, std::unique_ptr<Expr>& rExpr, std::unique_ptr<IType> rType)
        : Stmt(token), rType(std::move(rType)),
        rExpr(rExpr == nullptr ? nullptr : std::move(rExpr))
{
}

void ReturnStmt::analyze() const
{
    if (symTable == nullptr) return;

    if (rExpr != nullptr)
    {
        rExpr->setSymbolTable(symTable);
        rExpr->setScope(scope);
        rExpr->setClassNode(currClass);
        rExpr->analyze();
    }

    if (const auto currFunc = symTable->getCurrFunc())
    {
        currFunc->setHasReturned(true);
        const auto expectedType = currFunc->getReturnType();
        
        if (rExpr == nullptr)
        {
            if (expectedType->toString() != "fermata") symTable->addError(std::make_unique<WrongReturnType>(token));
        }
        else
        {
            if (*expectedType != *(rExpr->getType())) symTable->addError(std::make_unique<WrongReturnType>(token));
        }
    }
}

std::string ReturnStmt::translateToCpp() const
{
    const std::string exprStr = rExpr == nullptr ? "" :  " " + rExpr->translateToCpp();
    return getTabs() + "return" + exprStr +  ";";
}

void ReturnStmt::setSymbolTable(SymbolTable* symTable) const
{
    Stmt::setSymbolTable(symTable);
    rExpr->setSymbolTable(symTable);
}
