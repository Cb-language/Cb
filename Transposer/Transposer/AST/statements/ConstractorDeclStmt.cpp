#include "ConstractorDeclStmt.h"

#include <ranges>
#include <algorithm>
#include <iostream>

#include "errorHandling/classErrors/ParentNotInitialized.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "ConstractorCallStmt.h"
#include "other/SymbolTable.h"
#include "symbols/Type/ClassType.h"
#include "other/Utils.h"

ConstractorDeclStmt::ConstractorDeclStmt(const Token& token, Scope* scope, const ClassNode* currClass, const std::wstring& className,
                                         const std::vector<Var>& args) : IFuncDeclStmt(token, scope, currClass),
    constractor(Constractor(args,  className)), parentCtorCall(nullptr)
{
}

void ConstractorDeclStmt::setParentCtorCall(std::vector<std::unique_ptr<Expr>> args)
{
    if (parentCtorCall != nullptr) return;
    parentCtorCall = std::make_unique<ConstractorCallStmt>(token, scope, funcDecl, currClass, currClass->getParent(), std::move(args));
}

void ConstractorDeclStmt::setBody(std::unique_ptr<BodyStmt> body)
{
    this->body = std::move(body);
}

void ConstractorDeclStmt::analyze() const
{
    if (this->body == nullptr) throw HowDidYouGetHere(token);
    if (parentCtorCall != nullptr) parentCtorCall->analyze();
    body->analyze();
}

std::string ConstractorDeclStmt::translateToCpp() const
{
    std::ostringstream oss;
    oss << Utils::wstrToStr(constractor.getClassName()) << "::" << constractor.translateToCpp();

    const ConstractorCallStmt* bodyParentCall = nullptr;
    if (parentCtorCall == nullptr && !body->getStmts().empty())
    {
        if (auto ctorCall = dynamic_cast<ConstractorCallStmt*>(body->getStmts()[0].get()))
        {
            if (currClass->getParent() != nullptr && ctorCall->getClassNode() == currClass->getParent())
            {
                bodyParentCall = ctorCall;
            }
        }
    }

    if (parentCtorCall != nullptr || bodyParentCall != nullptr)
    {
        std::string str = parentCtorCall ? parentCtorCall->translateToCpp() : bodyParentCall->translateToCpp();
        
        // Remove tabs and semicolon for initializer list context
        str = Utils::removeAllFirstTabs(str);
        str.erase(
            std::ranges::remove(str, ';').begin(),
            str.end()
        ); 

        oss << " : " << str;
    }

    const std::string tabs = getTabs(-1);
    oss << std::endl << tabs << "{" << std::endl;

    bool skipFirst = (bodyParentCall != nullptr);
    for (size_t i = 0; i < body->getStmts().size(); ++i)
    {
        if (i == 0 && skipFirst) continue;
        
        std::string temp = body->getStmts()[i]->translateToCpp();
        oss << getTabs() << Utils::removeFirstTabs(temp) << std::endl;
    }

    oss << tabs << "}" << std::endl;

    return oss.str();
}

std::string ConstractorDeclStmt::translateToH() const
{
    return constractor.translateToCpp() + ";";
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
