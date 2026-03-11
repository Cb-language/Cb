#include "ObjCreationStmt.h"

#include "class/ClassNode.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "../../errorHandling/classErrors/InstantiateAbstractClass.h"
#include "symbols/Type/ClassType.h"

ObjCreationStmt::ObjCreationStmt(const Token& token,
                                 const ClassNode* classNode, const bool hasStartingValue, std::unique_ptr<ConstractorCallStmt> startingValue,
                                 const Var& var)
: VarDeclStmt(token, hasStartingValue, std::move(startingValue), var),
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
            if (clsPtr->getClassNode()->isAbstract())
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

    const std::string className = translateFQNtoString(classNode->getClass().getClassName());
    const std::string classSafePtr = "SafePtr<" + className + ">";

    oss << getTabs() << classSafePtr << " " << translateFQNtoString(var.getName()) << " = " << classSafePtr << "(";

    if (hasStartingValue && startingValue != nullptr) oss  << startingValue->translateToCpp();
    else oss << className << "()";

    oss << ");";

    return oss.str();
}
