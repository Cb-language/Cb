#include "ConstructorDeclStmt.h"

#include <ranges>
#include <algorithm>
#include <iostream>

#include "errorHandling/how/HowDidYouGetHere.h"
#include "ConstractorCallStmt.h"
#include "other/SymbolTable.h"
#include "symbols/Type/ClassType.h"
#include "other/Utils.h"

ConstructorDeclStmt::ConstructorDeclStmt(const Token& token, const FQN& className, const std::vector<Var>& args) : IFuncDeclStmt(token),
    constractor(Constractor(args,  className)), parentCtorCall(nullptr)
{
}

void ConstructorDeclStmt::setParentCtorCall(std::vector<std::unique_ptr<Expr>> args)
{
    if (parentCtorCall != nullptr) return;
    parentCtorCall = std::make_unique<ConstractorCallStmt>(token, std::move(args));
}

void ConstructorDeclStmt::setBody(std::unique_ptr<BodyStmt> body)
{
    this->body = std::move(body);
}

void ConstructorDeclStmt::analyze() const
{
    if (this->body == nullptr) throw HowDidYouGetHere(token);
    if (parentCtorCall != nullptr) parentCtorCall->analyze();
    body->analyze();
}

std::string ConstructorDeclStmt::translateToCpp() const
{
    std::ostringstream oss;
    oss << translateFQNtoString(constractor.getClassName()) << "::" << constractor.translateToCpp();

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
        const std::string temp = stmt->translateToCpp();
        oss << getTabs() << Utils::removeFirstTabs(temp) << std::endl;
    }

    oss << tabs << "}" << std::endl;

    return oss.str();
}

std::string ConstructorDeclStmt::translateToH() const
{
    return constractor.translateToCpp() + ";";
}

const FQN& ConstructorDeclStmt::getName() const
{
    return constractor.getClassName();
}

std::unique_ptr<IType> ConstructorDeclStmt::getReturnType() const
{
    return std::make_unique<ClassType>(constractor.getClassName());
}

const Constractor& ConstructorDeclStmt::getConstractor() const
{
    return constractor;
}
