#include "ObjInstanceStmt.h"

#include "class/ClassNode.h"
#include "errorHandling/classErrors/IllegalFieldName.h"
#include "errorHandling/how/HowDidYouGetHere.h"

ObjInstanceStmt::ObjInstanceStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, ClassNode* currClass,
                                 const ClassNode* classNode, const bool& isFuncCall, const std::wstring& fieldName)
    : Stmt(token, scope, funcDecl, currClass), classNode(classNode), isFuncCall(isFuncCall), fieldName(fieldName)
{
}

void ObjInstanceStmt::analyze() const
{
    if (classNode == nullptr) throw HowDidYouGetHere(token);

    if (classNode->isLegal(fieldName, ))

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
