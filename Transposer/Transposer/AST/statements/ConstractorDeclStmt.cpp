#include "ConstractorDeclStmt.h"

#include "errorHandling/how/HowDidYouGetHere.h"

ConstractorDeclStmt::ConstractorDeclStmt(const Token& token, Scope* scope, ClassNode* currClass, const std::wstring& className,
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
    body->analyze();
}

std::string ConstractorDeclStmt::translateToCpp() const
{
    return Utils::wstrToStr(constractor.getClassName()) + "::" + constractor.translateToCpp() + "\n" + body->translateToCpp() + "\n";
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
    return std::make_unique<Type>(constractor.getClassName()); // TODO: replace this with class type
}

const Constractor& ConstractorDeclStmt::getConstractor() const
{
    return constractor;
}
