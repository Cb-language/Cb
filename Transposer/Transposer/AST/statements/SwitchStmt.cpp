#include "SwitchStmt.h"

#include "errorHandling/semanticErrors/IllegalSwitchVar.h"

SwitchStmt::SwitchStmt(const Token& token, Var var, std::vector<std::unique_ptr<CaseStmt>>& cases)
    : Stmt(token), var(std::move(var)), cases(std::move(cases))
{
}

void SwitchStmt::setVar(const Var& var)
{
    this->var = var;
}

void SwitchStmt::analyze() const
{
    for (const auto& c : cases)
    {
        c->analyze();
    }

    if (!var.isNumberable())
    {
        throw IllegalSwitchVar(token, translateFQNtoString(var.getName()));
    }
}

std::string SwitchStmt::translateToCpp() const
{
    std::ostringstream os;
    os << getTabs() << "switch (" << translateFQNtoString(var.getName()) << ")\n";
    os << getTabs() << "{\n";
    for (const auto& c : cases)
    {
        os << c->translateToCpp();
    }
    os << getTabs() << "}\n";
    return os.str();
}
