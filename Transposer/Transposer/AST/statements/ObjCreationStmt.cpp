#include "ObjCreationStmt.h"

#include "class/ClassNode.h"
#include "errorHandling/how/HowDidYouGetHere.h"

ObjCreationStmt::ObjCreationStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass,
                                 const ClassNode* classNode, const std::wstring& name)
        : Stmt(token, scope, funcDecl, currClass),
         hasCtor(classNode != nullptr && !classNode->getClass().getConstractors().empty()), classNode(classNode),
         name(name)
{
}

void ObjCreationStmt::analyze() const
{

}

std::string ObjCreationStmt::translateToCpp() const
{
    std::ostringstream oss;
    if (classNode == nullptr) throw HowDidYouGetHere(token);
    oss << getTabs() + Utils::wstrToStr(classNode->getClass().getClassName()) + " " + Utils::wstrToStr(name);
    if (hasCtor)
    {
        oss << "()";
    }
    oss << ";";
    return oss.str();
}

