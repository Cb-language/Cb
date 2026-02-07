#include "ConstractorDeclStmt.h"

#include "errorHandling/how/HowDidYouGetHere.h"

ConstractorDeclStmt::ConstractorDeclStmt(const Token& token, Scope* scope, const std::wstring& className,
                                         const std::vector<Var>& args) : Stmt(token, scope), constractor(Constractor(args,  className))
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
    return Utils::wstrToStr(constractor.getClassName()) + "::" + constractor.translateToCpp() + "\n" + body->translateToCpp();
}

std::string ConstractorDeclStmt::translateToH() const
{
    return constractor.translateToCpp();
}

const Constractor& ConstractorDeclStmt::getConstractor() const
{
    return constractor;
}
