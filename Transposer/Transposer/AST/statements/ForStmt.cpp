#include "ForStmt.h"

#include "errorHandling/how/HowDidYouGetHere.h"
#include "other/SymbolTable.h"

ForStmt::ForStmt(const Token& token,
                 std::unique_ptr<BodyStmt> body, const bool isIncreasing,
                 std::unique_ptr<Expr> startExpr, std::unique_ptr<Expr> stepExpr, std::unique_ptr<Expr> stopExpr, const std::string& varName)
    : Stmt(token), body(std::move(body)), isIncreasing(isIncreasing),
    startExpr(std::move(startExpr)), stepExpr(std::move(stepExpr)), stopExpr(std::move(stopExpr)), varName(varName)
{
}

void ForStmt::analyze() const
{
    if (symTable == nullptr) return;

    if (startExpr != nullptr)
    {
        startExpr->setSymbolTable(symTable);
        startExpr->setScope(scope);
        startExpr->setClassNode(currClass);
        startExpr->analyze();
    }

    if (stepExpr != nullptr)
    {
        stepExpr->setSymbolTable(symTable);
        stepExpr->setScope(scope);
        stepExpr->setClassNode(currClass);
        stepExpr->analyze();
    }

    if (stopExpr != nullptr)
    {
        stopExpr->setSymbolTable(symTable);
        stopExpr->setScope(scope);
        stopExpr->setClassNode(currClass);
        stopExpr->analyze();
    }

    if (body == nullptr)
    {
        symTable->addError(std::make_unique<HowDidYouGetHere>(token));
    }
    symTable->enterScope(true, true);

    const FQN fqn = {varName};
    const Var loopVar(std::make_unique<PrimitiveType>(Primitive::TYPE_DEGREE), fqn);
    symTable->addVar(loopVar, token);

    body->setSymbolTable(symTable);
    body->setScope(symTable->getCurrScope());
    body->setClassNode(currClass);
    
    // Manual analysis of body statements to skip BodyStmt's enterScope (which we already did)
    for (const auto& s : body->getStmts())
    {
        s->setSymbolTable(symTable);
        s->setScope(symTable->getCurrScope());
        s->setClassNode(currClass);
        s->analyze();
    }

    symTable->exitScope();
}

std::string ForStmt::translateToCpp() const
{
    std::ostringstream oss;
    const std::string startStr = startExpr->translateToCpp();
    const std::string stepStr = stepExpr != nullptr ? stepExpr->translateToCpp() : "Primitive<int>(1)";
    const std::string stopStr = stopExpr != nullptr ? stopExpr->translateToCpp() : "Primitive<int>(0)";
    const std::string nameStr = varName;
    std::string stepFullStr;

    if (stepStr == "1")
    {
        stepFullStr = nameStr + (isIncreasing ? "++" : "--");
    }
    else
    {
        stepFullStr = nameStr + (isIncreasing ? " += " : " -= ") + stepStr;
    }

    oss << getTabs() << "for (Primitive<int> " << nameStr << " = " << startStr << "; ";
    oss << nameStr << (isIncreasing ? " < " : " >= ") << stopStr << "; ";
    oss << stepFullStr << ")" << std::endl;
    oss << body->translateToCpp();
    return oss.str();
}

void ForStmt::setSymbolTable(SymbolTable* symTable) const
{
    Stmt::setSymbolTable(symTable);
    if (startExpr != nullptr) startExpr->setSymbolTable(symTable);
    if (stepExpr != nullptr) stepExpr->setSymbolTable(symTable);
    if (stopExpr != nullptr) stopExpr->setSymbolTable(symTable);
    body->setSymbolTable(symTable);
}
