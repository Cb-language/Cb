#include "SwitchStmt.h"

SwitchStmt::SwitchStmt(Scope *scope, FuncDeclStmt *funcDecl, std::optional<Var> variable, std::vector<std::unique_ptr<CaseStmt>> cases) : Stmt(scope, funcDecl), variable(std::move(variable))
{
    for (auto& c : cases)
    {
        cases.push_back(std::move(c));
    }
}

bool SwitchStmt::isLegal() const
{
    return variable != nullptr;
}

std::string SwitchStmt::translateToCpp() const
{
    std::ostringstream os;
    os << getTabs() << "switch (" << Utils::wstrToStr(variable->getName()) << ")\n";
    os << "{\n";
    for (const auto& c : cases)
    {
        os << c->translateToCpp();
    }
    os << "}\n";
    return os.str();
}
