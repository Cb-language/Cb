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
                                         const std::vector<Var>& args) : IFuncDeclStmt(token, scope, currClass),
    constractor(Constractor(args,  className)), parentCtorCall(nullptr)
{
}

ConstractorDeclStmt::ConstractorDeclStmt(const Token& token, Scope* scope, const ClassNode* currClass,
    const std::wstring& className, const std::vector<Var>& args, std::vector<std::unique_ptr<Expr>> parentArgs) : IFuncDeclStmt(token, scope, currClass),
    constractor(Constractor(args,  className)),
    parentCtorCall(std::make_unique<ConstractorCallStmt>(token, scope, funcDecl, currClass, currClass->getParent(), std::move(parentArgs)))
{
}

void ConstractorDeclStmt::setBody(std::unique_ptr<BodyStmt> body)
{
    this->body = std::move(body);
}

void ConstractorDeclStmt::analyze() const
{
    if (this->body == nullptr) throw HowDidYouGetHere(token);

    body->analyze();
}

std::string ConstractorDeclStmt::translateToCpp() const
{
    std::ostringstream oss;
    oss << Utils::wstrToStr(constractor.getClassName()) << "::" << constractor.translateToCpp();

    if (parentCtorCall != nullptr)
    {
        oss << " : " << parentCtorCall->translateToCpp();
    }

    const std::string tabs = getTabs();
    oss << std::endl << tabs << "{" << std::endl;

    bool first = true;
    for (const auto& stmt : body->getStmts())
    {
        if (!first)
        {
            oss << std::endl;
        }
        oss << stmt->translateToCpp();
        first = false;
    }

    oss << std::endl << tabs << "}" << std::endl;

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
