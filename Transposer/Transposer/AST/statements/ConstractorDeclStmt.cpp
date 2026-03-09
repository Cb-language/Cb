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

ConstractorDeclStmt::ConstractorDeclStmt(const Token& token, const std::string& className,
                                         const std::vector<Var>& args, ClassDeclStmt* classDecl) : IFuncDeclStmt(token, classDecl),
    constractor(Constractor(args,  className)), parentCtorCall(nullptr)
{
}

void ConstractorDeclStmt::setParentCtorCall(std::vector<std::unique_ptr<Expr>> args)
{
    if (parentCtorCall != nullptr) return;
    parentCtorCall = std::make_unique<ConstractorCallStmt>(token, funcDecl, std::move(args));
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
    oss << constractor.getClassName() << "::" << constractor.translateToCpp();

    if (parentCtorCall != nullptr)
    {
        std::string str = parentCtorCall->translateToCpp();
        str.erase(
            std::ranges::remove(str, ';').begin(),
            str.end()
        ); // removing ;

        oss << " : " << Utils::removeFirstTabs(str);
    }

    const std::string tabs = getTabs(-1);
    oss << std::endl << tabs << "{" << std::endl;

    for (const auto& stmt : body->getStmts())
    {
        std::string temp = stmt->translateToCpp();
        oss << getTabs() << Utils::removeFirstTabs(temp) << std::endl;
    }

    oss << tabs << "}" << std::endl;

    return oss.str();
}

std::string ConstractorDeclStmt::translateToH() const
{
    return constractor.translateToCpp() + ";";
}

std::string ConstractorDeclStmt::getName() const
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
