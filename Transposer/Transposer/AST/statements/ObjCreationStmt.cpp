#include "ObjCreationStmt.h"

#include "class/ClassNode.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "symbols/Type/ClassType.h"
#include "../../errorHandling/classErrors/InstantiateAbstractClass.h"

ObjCreationStmt::ObjCreationStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass,
                                 const ClassNode* classNode, const bool hasStartingValue, std::unique_ptr<ConstractorCallStmt> startingValue,
                                 const Var& var)
: VarDeclStmt(token, scope, funcDecl, currClass, hasStartingValue, std::move(startingValue), var),
      classNode(classNode)
{
    if (classNode == nullptr) throw HowDidYouGetHere(token);
}

void ObjCreationStmt::analyze() const
{
    if (classNode->getClass().isAbstract())
    {
        throw InstantiateAbstractClass(token);
    }
    VarDeclStmt::analyze();
}

std::string ObjCreationStmt::translateToCpp() const
{
    std::ostringstream oss;
    if (classNode == nullptr) throw HowDidYouGetHere(token);

    const std::string className = Utils::wstrToStr(classNode->getClass().getClassName());
    const std::string classSafePtr = "SafePtr<" + className + ">";

    oss << getTabs() << classSafePtr << " " << Utils::wstrToStr(var.getName()) << " = " << classSafePtr << "(";

    if (hasStartingValue && startingValue != nullptr) oss  << startingValue->translateToCpp();
    else oss << className << "()";

    oss << ");";

    return oss.str();
}
