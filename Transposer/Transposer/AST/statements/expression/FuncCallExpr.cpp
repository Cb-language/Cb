#include "FuncCallExpr.h"

#include <sstream>

#include "AST/statements/FuncDeclStmt.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "errorHandling/semanticErrors/CallWithoutCopyright.h"

FuncCallExpr::FuncCallExpr(const Token& token,
    const FQN& name, std::vector<std::unique_ptr<Expr>> args, const bool needsSemicolon)
    : Call(token), name(name), type(std::make_unique<PrimitiveType>(Primitive::TYPE_FERMATA)),
      needsSemicolon(needsSemicolon), funcDecl(nullptr)
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

    if (translateFQNtoString(funcDecl->getName()) == "prelude")
    {
        return; // main doesnt have to copyright
    }

    if (funcDecl->getName() == name)
    {
        return; // a func doesn't have to copyright itself
    }

    for (const auto& credit : funcDecl->getCredited())
    {
        if (credit->getName() == translateFQNtoString(name))
        {
            return;
        }
    }
   throw CallWithoutCopyright(token, translateFQNtoString(name));
}

std::string FuncCallExpr::translateToCpp() const
{
    std::ostringstream oss;
    bool first = true;

    if (needsSemicolon && !isClassItem)
    {
        oss << getTabs();
    }

    oss << translateFQNtoString(name) << "(";

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

    if (needsSemicolon)
    {
        oss << ";";
    }

    return oss.str();
}

void FuncCallExpr::setType(std::unique_ptr<IType> type)
{
    this->type = std::move(type);
}

void FuncCallExpr::setClassDecl(IFuncDeclStmt& decl)
{
    this->funcDecl = &decl;
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

const FQN& FuncCallExpr::getName() const
{
    return name;
}

std::string FuncCallExpr::toString() const
{
    return translateFQNtoString(name);
}

void FuncCallExpr::setNeedsSemicolon(const bool needsSemicolon)
{
    this->needsSemicolon = needsSemicolon;
}
