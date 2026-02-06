#include "ConstractorDeclStmt.h"

ConstractorDeclStmt::ConstractorDeclStmt(const Token& token, Scope* scope, const std::wstring& className,
    const std::vector<Var>& args, std::unique_ptr<BodyStmt>& body) : Stmt(token, scope), body(std::move(body)), constractor(Constractor(args,  className))
{
}

void ConstractorDeclStmt::analyze() const
{
    body->analyze();
}

std::string ConstractorDeclStmt::translateToCpp() const
{
    return constractor.translateToCpp() + "\n" + body->translateToCpp();
}

std::string ConstractorDeclStmt::translateToH() const
{
    return constractor.translateToCpp() + ";";
}
