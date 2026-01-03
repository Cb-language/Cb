#include "FuncCallExpr.h"

#include <sstream>

#include "AST/statements/FuncDeclStmt.h"

FuncCallExpr::FuncCallExpr(const Token& token, Scope* scope, FuncDeclStmt* funcDecl, const std::wstring& name,std::vector<std::unique_ptr<Expr>> args, const bool isStmt)
    : Call(token, scope, funcDecl), name(name), type(std::make_unique<Type>(L"fermata")), isStmt(isStmt)
{
    for (auto& arg : args)
    {
        this->args.emplace_back(std::move(arg));
    }
}

std::unique_ptr<IType> FuncCallExpr::getType() const
{
    return type->copy();
}

bool FuncCallExpr::isLegal() const
{
    if (funcDecl == nullptr)
    {
        return false;
    }

    if (funcDecl->getName() == L"prelude")
    {
        return true; // main doesnt have to copy right
    }

    if (funcDecl->getName() == name)
    {
        return true; // a func doesn't have to copyright itself
    }

    for (const auto& credit : funcDecl->getCredited())
    {
        if (credit->getName() == name)
        {
            return true;
        }
    }
    return false;
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

void FuncCallExpr::setType(std::unique_ptr<IType> type)
{
    this->type = std::move(type);
}

bool FuncCallExpr::isLegalCall(const Func& func) const
{
    if (name != func.getFuncName() || args.size() != func.getArgs().size())
    {
        return false;
    }

    for (int i = 0; i < func.getArgs().size(); i++)
    {
        if (*(func.getArgs()[i].getType()) != *(args[i]->getType()))
        {
            return false;
        }
    }
    return true;
}
