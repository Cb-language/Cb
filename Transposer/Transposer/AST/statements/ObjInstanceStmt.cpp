#include "ObjInstanceStmt.h"

#include "AST/abstract/IFuncDeclStmt.h"

ObjInstanceStmt::ObjInstanceStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl,
    const Class& _class, const std::wstring& name) : Stmt(token, scope, funcDecl), hasCtor(!(_class.getConstractors().empty())), _class(_class), name(name)
{
}

void ObjInstanceStmt::analyze() const
{

}

std::string ObjInstanceStmt::translateToCpp() const
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

