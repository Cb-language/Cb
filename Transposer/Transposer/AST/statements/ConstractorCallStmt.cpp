#include "ConstractorCallStmt.h"

#include "errorHandling/classErrors/AccessError.h"
#include "errorHandling/classErrors/InvalidCtorArgs.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "other/Utils.h"
#include "symbols/Type/ClassType.h"

ConstractorCallStmt::ConstractorCallStmt(const Token& token, IFuncDeclStmt* funcDecl, std::vector<std::unique_ptr<Expr>> args, ClassDeclStmt* classDecl)
        : Expr(token, funcDecl, classDecl)
{
    for (auto& arg : args)
    {
        this->args.emplace_back(std::move(arg));
    }
}

std::unique_ptr<IType> ConstractorCallStmt::getType() const
{
    return std::make_unique<ClassType>(currClass);
}

void ConstractorCallStmt::analyze() const
{
    for (const auto& [accessType, ctor] : currClass->getClass().getConstractors())
    {
        auto ctorArgs = ctor.getArgs();
        if (args.size() != ctorArgs.size()) continue;

        bool differ = false;

        for (int i = 0; i < args.size() && !differ; i++)
        {
            if (*args[i]->getType() != *ctorArgs[i].getType()) differ = true;
        }

        if (!differ)
        {
            if (currClass->isLegalAccess(accessType, currClass)) return;

            throw AccessError(token, currClass->getClass().getClassName(), currClass->getClass().getClassName() + "_call");
        }
    }

    throw InvalidCtorArgs(token);
}

std::string ConstractorCallStmt::translateToCpp() const
{
    std::ostringstream oss;

    if (needsSemicolon)
    {
        oss << getTabs();
    }

    oss << currClass->getClass().getClassName() << "(";

    bool first = true;
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

void ConstractorCallStmt::setNeedsSemicolon(const bool needsSemicolon)
{
    this->needsSemicolon = needsSemicolon;
}

const ClassNode* ConstractorCallStmt::getClassNode() const
{
    return currClass;
}
