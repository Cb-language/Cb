#include "FuncCallExpr.h"

#include <sstream>

FuncCallExpr::FuncCallExpr(Scope* scope, FuncDeclStmt* funcDecl, const std::wstring& name,std::vector<std::unique_ptr<Expr>> args, const bool isStmt)
    : Expr(scope, funcDecl), name(name), type(Type(L"fermata")), isStmt(isStmt)
{
    for (auto& arg : args)
    {
        this->args.emplace_back(std::move(arg));
    }
}
Type FuncCallExpr::getType() const
{
    return type;
}

bool FuncCallExpr::isLegal() const
{
    return true;
}

std::string FuncCallExpr::translateToCpp() const
{
    std::ostringstream oss;
    bool first = true;

    if (isStmt)
    {
        oss << getTabs();
    }

    oss << Utils::wstrToStr(name) << "(";

    for (const auto& arg : args)
    {
        if (!first)
        {
            oss << ", ";
        }
        first = false;

        oss << arg->translateToCpp();
    }

    oss << ")";

    if (isStmt)
    {
        oss << ";";
    }

    return oss.str();
}

void FuncCallExpr::setType(Type type)
{
    this->type = type;
}

bool FuncCallExpr::isLegalCall(const Func& func) const
{
    auto fArgs = func.getArgs();

    if (name != func.getFuncName() || args.size() != fArgs.size())
    {
        return false;
    }

    for (int i = 0; i < fArgs.size(); i++)
    {
        if (fArgs[i].getType() != args[i]->getType())
        {
            return false;
        }
    }
    return true;
}
