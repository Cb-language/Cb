#include "SwitchStmt.h"

#include "errorHandling/semanticErrors/IllegalSwitchVar.h"
#include "other/SymbolTable.h"

SwitchStmt::SwitchStmt(const Token& token, std::unique_ptr<Expr> expr, std::vector<std::unique_ptr<CaseStmt>>& cases)
    : Stmt(token), expr(std::move(expr)), cases(std::move(cases))
{
}

void SwitchStmt::setExpr(std::unique_ptr<Expr> expr)
{
    this->expr = std::move(expr);
}

void SwitchStmt::analyze() const
{
    if (symTable == nullptr) return;

    expr->setSymbolTable(symTable);
    expr->setScope(scope);
    expr->setClassNode(currClass);
    expr->analyze();

    if (!expr->getType()->isNumberable())
    {
        symTable->addError(std::make_unique<IllegalSwitchVar>(token, expr->translateToCpp()));
    }

    for (const auto& c : cases)
    {
        c->setSymbolTable(symTable);
        c->setScope(scope);
        c->setClassNode(currClass);
        c->analyze();
    }
}

std::string SwitchStmt::translateToCpp() const
{
    std::ostringstream os;
    os << getTabs() << "switch (" << expr->translateToCpp() << ")\n";
    os << getTabs() << "{\n";
    for (const auto& c : cases)
    {
        os << c->translateToCpp();
    }
    os << getTabs() << "}\n";
    return os.str();
}
