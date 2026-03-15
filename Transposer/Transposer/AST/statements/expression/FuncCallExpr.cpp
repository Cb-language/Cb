#include "FuncCallExpr.h"

#include <sstream>

#include "AST/statements/FuncDeclStmt.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "errorHandling/semanticErrors/CallWithoutCopyright.h"
#include "other/SymbolTable.h"
#include "errorHandling/semanticErrors/IllegalCall.h"

FuncCallExpr::FuncCallExpr(const Token& token,
    const FQN& name, std::vector<std::unique_ptr<Expr>> args, const bool needsSemicolon)
    : VarReference(token), name(name), type(std::make_unique<PrimitiveType>(Primitive::TYPE_FERMATA)),
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
    if (symTable == nullptr) return;

    for (const auto& arg : args)
    {
        arg->setSymbolTable(symTable);
        arg->setScope(scope);
        arg->setClassNode(currClass);
        arg->analyze();
    }

    if (auto t = symTable->getCallType(this, currClass))
    {
        const_cast<FuncCallExpr*>(this)->type = std::move(t);
    }
    else
    {
        symTable->addError(std::make_unique<IllegalCall>(token, translateFQNtoString(this->getName())));
    }

    if (funcDecl == nullptr)
    {
        return;
    }

    if (funcDecl->getIsMethod())
    {
        return; // methods don't need to be credited
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
   symTable->addError(std::make_unique<CallWithoutCopyright>(token, translateFQNtoString(name)));
}

std::string FuncCallExpr::translateToCpp() const
{
    std::ostringstream oss;
    bool first = true;

    if (needsSemicolon && !isClassItem)
    {
        oss << getTabs();
    }

    if (targetClass != nullptr && !name.empty() && translateFQNtoString({name[0]}) == translateFQNtoString(targetClass->getClass().getClassName()))
    {
        // Static call: Animal::kingdom()
        oss << translateFQNtoString({name[0]}) << "::" << name.back() << "(";
    }
    else
    {
        // Member call (->) or global call
        oss << translateFQNtoString(name) << "(";
    }

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

void FuncCallExpr::setTargetClass(const ClassNode* targetClass)
{
    this->targetClass = targetClass;
}

bool FuncCallExpr::isLegalCall(const Func& func) const
{
    if (name != func.getFuncName() || args.size() != func.getArgs().size())
    {
        return false;
    }

    return argsMatch(func);
}

bool FuncCallExpr::argsMatch(const Func& func) const
{
    if (args.size() != func.getArgs().size())
    {
        return false;
    }

    for (size_t i = 0; i < args.size(); i++)
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
