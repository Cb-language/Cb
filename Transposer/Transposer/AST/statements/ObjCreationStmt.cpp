#include "ObjInstanceStmt.h"

#include "AST/abstract/IFuncDeclStmt.h"

ObjCreationStmt::ObjCreationStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl,
    const std::optional<Class>& _class, const std::wstring& name) : Stmt(token, scope, funcDecl), hasCtor(!(_class->getConstractors().empty())), _class(_class.value), name(name)
{
}

void ObjCreationStmt::analyze() const
{

}

std::string ObjCreationStmt::translateToCpp() const
{
    std::ostringstream oss;
    oss << getTabs() + Utils::wstrToStr(_class.getClassName()) + " " + Utils::wstrToStr(name);
    if (hasCtor)
    {
        oss << "()";
    }
    oss << ";";
    return oss.str();
}

