#include "ConstractorCallStmt.h"

#include "errorHandling/classErrors/AccessError.h"
#include "errorHandling/classErrors/InvalidCtorArgs.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "other/Utils.h"
#include "symbols/Type/ClassType.h"
#include "other/SymbolTable.h"

ConstractorCallStmt::ConstractorCallStmt(const Token& token, std::vector<std::unique_ptr<Expr>> args)
        : Expr(token)
{
    for (auto& arg : args)
    {
        this->args.emplace_back(std::move(arg));
    }
}

std::unique_ptr<IType> ConstractorCallStmt::getType() const
{
    if (targetClass != nullptr) return std::make_unique<ClassType>(targetClass->getClass().getClassName());
    return std::make_unique<ClassType>(currClass->getClass().getClassName());
}

void ConstractorCallStmt::analyze() const
{
    const ClassNode* target = targetClass != nullptr ? targetClass : currClass;
    if (target == nullptr) symTable->addError(std::make_unique<HowDidYouGetHere>(token));

    for (const auto& arg : args)
    {
        arg->setSymbolTable(symTable);
        arg->setScope(scope);
        arg->setClassNode(currClass);
        arg->analyze();
    }

    for (const auto& [accessType, ctor] : target->getClass().getConstractors())
    {
        const auto& ctorArgs = ctor.getArgs();
        if (args.size() != ctorArgs.size()) continue;

        bool differ = false;

        for (int i = 0; i < args.size() && !differ; i++)
        {
            if (*args[i]->getType() != *ctorArgs[i].getType()) differ = true;
        }

        if (!differ)
        {
            if (target->isLegalAccess(accessType, currClass)) return;

            symTable->addError(std::make_unique<AccessError>(token, translateFQNtoString(target->getClass().getClassName()), translateFQNtoString(target->getClass().getClassName()) + "_call"));
        }
    }

    symTable->addError(std::make_unique<InvalidCtorArgs>(token));
}

std::string ConstractorCallStmt::translateToCpp() const
{
    std::ostringstream oss;
    const ClassNode* target = targetClass != nullptr ? targetClass : currClass;

    if (needsSemicolon)
    {
        oss << getTabs();
    }

    oss << translateFQNtoString(target->getClass().getClassName()) << "(";

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

void ConstractorCallStmt::setSymbolTable(SymbolTable* symTable) const
{
    Stmt::setSymbolTable(symTable);
    for (const auto& arg : args) arg->setSymbolTable(symTable);
}

void ConstractorCallStmt::setNeedsSemicolon(const bool needsSemicolon)
{
    this->needsSemicolon = needsSemicolon;
}

const ClassNode* ConstractorCallStmt::getClassNode() const
{
    return targetClass != nullptr ? targetClass : currClass;
}

void ConstractorCallStmt::setTargetClass(const ClassNode* targetClass)
{
    this->targetClass = targetClass;
}
