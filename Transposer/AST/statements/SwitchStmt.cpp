#include "SwitchStmt.h"

SwitchStmt::SwitchStmt(const Token& token, Scope *scope, FuncDeclStmt *funcDecl, Var var, std::vector<std::unique_ptr<CaseStmt>>& cases) : Stmt(token, scope, funcDecl), var(std::move(var)), cases(std::move(cases))
{
}

bool SwitchStmt::isLegal() const
{
    for (const auto& c : cases)
    {
        if (!c->isLegal())
        {
            return false;
        }
    }
    return var.isNumberable();
}

std::string SwitchStmt::translateToCpp() const
{
    std::ostringstream os;
    os << getTabs() << "switch (" << Utils::wstrToStr(var.getName()) << ")\n";
    os << getTabs() << "{\n";
    for (const auto& c : cases)
    {
        os << c->translateToCpp();
    }
    os << getTabs() << "}\n";
    return os.str();
}
