#include "ObjInstanceStmt.h"

#include "errorHandling/classErrors/IllegalFieldName.h"

ObjInstanceStmt::ObjInstanceStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl,
                                 std::unique_ptr<Class>& _class, const bool& isFuncCall, const std::wstring& fieldName)
    : Stmt(token, scope, funcDecl), _class(std::move(_class)), isFuncCall(isFuncCall), fieldName(fieldName)
{
}

void ObjInstanceStmt::analyze() const
{
    if (!_class->hasField(fieldName))
    {
        throw IllegalFieldName(token);
    }
    if (_class->hasMethod(fieldName) && isFuncCall)
    {
        return;
    }
    if (_class->hasField(fieldName) && !isFuncCall)
    {
        return;
    }
    throw IllegalFieldName(token);
}

std::string ObjInstanceStmt::translateToCpp() const
{
    std::ostringstream oss;
    oss << getTabs() << Utils::wstrToStr(_class->getClassName()) << "->" << Utils::wstrToStr(fieldName);
    if (isFuncCall)
    {
        oss << "()";
    }
    oss << ";";
    return oss.str();
}
