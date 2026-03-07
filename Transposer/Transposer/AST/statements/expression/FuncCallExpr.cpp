#include "FuncCallExpr.h"

#include <sstream>

#include "AST/statements/FuncDeclStmt.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "errorHandling/semanticErrors/CallWithoutCopyright.h"

FuncCallExpr::FuncCallExpr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass,
    const std::string& name,std::vector<std::unique_ptr<Expr>> args, const bool isStmt)
    : Call(token, scope, funcDecl, currClass), name(name), type(std::make_unique<Type>("fermata")), isStmt(isStmt)
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

void FuncCallExpr::analyze() const
{
    if (funcDecl == nullptr)
    {
        throw HowDidYouGetHere(token);
    }

    if (funcDecl->getName() == "prelude")
    {
        return; // main doesnt have to copy right
    }

    if (funcDecl->getName() == name)
    {
        return; // a func doesn't have to copyright itself
    }

    for (const auto& credit : funcDecl->getCredited())
    {
        if (credit->getName() == name)
        {
            return;
        }
    }
   throw CallWithoutCopyright(token, name);
}

std::string FuncCallExpr::translateToCpp() const
{
    std::ostringstream oss;
    bool first = true;

    if (isStmt && !isClassItem)
    {
        oss << getTabs();
    }

    oss << name << "(";

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

const Token& FuncCallExpr::getToken() const
{
    return token;
}

const std::string& FuncCallExpr::getName() const
{
    return name;
}

std::string FuncCallExpr::toString() const
{
    return name;
}

void FuncCallExpr::setIsStmt(const bool isStmt)
{
    this->isStmt = isStmt;
}
