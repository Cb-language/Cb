#include "SwitchStmt.h"

SwitchStmt::SwitchStmt(Scope *scope, FuncDeclStmt *funcDecl, std::unique_ptr<Var> variable) : Stmt(scope, funcDecl), variable(std::move(variable))
{
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
