#include "ObjCreationStmt.h"

#include "class/ClassNode.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "../../errorHandling/classErrors/InstantiateAbstractClass.h"
#include "symbols/Type/ClassType.h"

ObjCreationStmt::ObjCreationStmt(const Token& token, IFuncDeclStmt* funcDecl,
                                 const ClassNode* classNode, const bool hasStartingValue, std::unique_ptr<ConstractorCallStmt> startingValue,
                                 const Var& var, ClassDeclStmt* classDecl)
: VarDeclStmt(token, funcDecl, hasStartingValue, std::move(startingValue), var, classDecl),
      classNode(classNode)
{
    if (classNode == nullptr) throw HowDidYouGetHere(token);
}

void ObjCreationStmt::analyze() const
{
    if (startingValue != nullptr)
    {
        const auto type = startingValue->getType()->copy();
        if (const auto clsPtr = dynamic_cast<ClassType*>(type.get()))
        {
            if (clsPtr->getClass()->isAbstract())
            {
                throw InstantiateAbstractClass(token);
            }
        }
    }
    VarDeclStmt::analyze();
}

std::string ObjCreationStmt::translateToCpp() const
{
    std::ostringstream oss;
    if (classNode == nullptr) throw HowDidYouGetHere(token);

    const std::string className = classNode->getClass().getClassName();
    const std::string classSafePtr = "SafePtr<" + className + ">";

    oss << getTabs() << classSafePtr << " " << var.getName() << " = " << classSafePtr << "(";

    if (hasStartingValue && startingValue != nullptr) oss  << startingValue->translateToCpp();
    else oss << className << "()";

    oss << ");";

    return oss.str();
}
