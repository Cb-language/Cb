#include "ConstractorCallStmt.h"

#include "errorHandling/classErrors/InvalidCtorArgs.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "symbols/Type/ClassType.h"

ConstractorCallStmt::ConstractorCallStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl,
                                         const ClassNode* currClass, const ClassNode* classNode, std::vector<std::unique_ptr<Expr>> args)
        : Expr(token, scope, funcDecl, currClass), classNode(classNode)
{
    if (classNode == nullptr) throw HowDidYouGetHere(token);
    for (auto& arg : args)
    {
        this->args.emplace_back(std::move(arg));
    }
}

std::unique_ptr<IType> ConstractorCallStmt::getType() const
{
    return std::make_unique<ClassType>(classNode);
}

void ConstractorCallStmt::analyze() const
{
    for (const auto& [isPublic, ctor] : classNode->getClass().getConstractors())
    {
        auto ctorArgs = ctor.getArgs();
        if (!isPublic || args.size() != ctorArgs.size()) continue;

        bool differ = false;

        for (int i = 0; i < args.size() && !differ; i++)
        {
            if (*args[i]->getType() != *ctorArgs[i].getType()) differ = true;
        }

        if (!differ) return;
    }

    throw InvalidCtorArgs(token);
}

std::string ConstractorCallStmt::translateToCpp() const
{
    std::ostringstream oss;

    oss << Utils::wstrToStr(classNode->getClass().getClassName()) << "(";

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
    return oss.str();
}
