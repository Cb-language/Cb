#include "ConstractorDeclStmt.h"

#include <ranges>
#include <iostream>

#include "errorHandling/classErrors/ParentNotInitialized.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "expression/ConstractorCallStmt.h"
#include "other/SymbolTable.h"
#include "symbols/Type/ClassType.h"
#include "other/Utils.h"

ConstractorDeclStmt::ConstractorDeclStmt(const Token& token, Scope* scope, const ClassNode* currClass, const std::wstring& className,
                                         const std::vector<Var>& args) : IFuncDeclStmt(token, scope, currClass), constractor(Constractor(args,  className))
{
}

void ConstractorDeclStmt::setBody(std::unique_ptr<BodyStmt> body)
{
    this->body = std::move(body);
}

void ConstractorDeclStmt::analyze() const
{
    if (this->body == nullptr) throw HowDidYouGetHere(token);

    if (currClass->getParent() != nullptr)
    {
        bool found = false;
        for (const auto& stmt : body->getStmts())
        {
            auto ctorCall = dynamic_cast<ConstractorCallStmt*>(stmt.get());
            if (ctorCall && ctorCall->getClassNode()->getClass().getClassName() == currClass->getParent()->getClass().getClassName())
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            bool hasDefaultCtor = false;

            if(currClass->getParent()->getClass().getClassName() == L"Object")
            {
                hasDefaultCtor = true;
            }
            else if (!currClass->getParent()->getClass().getConstractors().empty())
            {
                for (const auto& ctor : currClass->getParent()->getClass().getConstractors() | std::views::values)
                {
                    if (ctor.getArgs().empty())
                    {
                        if (currClass->getParent()->isLegal(ctor, currClass))
                        {
                            hasDefaultCtor = true;
                        }
                        break;
                    }
                }
            }

            if (!hasDefaultCtor)
            {
                throw ParentNotInitialized(token, Utils::wstrToStr(currClass->getClass().getClassName()), Utils::wstrToStr(currClass->getParent()->getClass().getClassName()));
            }
        }
    }

    body->analyze();
}

std::string ConstractorDeclStmt::translateToCpp() const
{
    std::string initList;
    ConstractorCallStmt* parentCall = nullptr;

    if (currClass->getParent() != nullptr)
    {
        for (const auto& stmt : body->getStmts())
        {
            auto ctorCall = dynamic_cast<ConstractorCallStmt*>(stmt.get());
            if (ctorCall && ctorCall->getClassNode()->getClass().getClassName() == currClass->getParent()->getClass().getClassName())
            {
                parentCall = ctorCall;
                initList = " : " + ctorCall->translateToCpp();
                break;
            }
        }
    }

    std::ostringstream oss;
    oss << Utils::wstrToStr(constractor.getClassName()) << "::" << constractor.translateToCpp() << initList << std::endl;

    const std::string tabs = getTabs();
    oss << tabs << "{\n";

    bool first = true;
    for (const auto& stmt : body->getStmts())
    {
        if (stmt.get() == parentCall) continue;

        if (!first)
        {
            oss << "\n";
        }
        oss << stmt->translateToCpp();
        first = false;
    }

    oss << "\n" << tabs << "}\n";

    return oss.str();
}

std::string ConstractorDeclStmt::translateToH() const
{
    return constractor.translateToCpp();
}

std::wstring ConstractorDeclStmt::getName() const
{
    return constractor.getClassName();
}

std::unique_ptr<IType> ConstractorDeclStmt::getReturnType() const
{
    return std::make_unique<ClassType>(SymbolTable::getClass(constractor.getClassName()));
}

const Constractor& ConstractorDeclStmt::getConstractor() const
{
    return constractor;
}
