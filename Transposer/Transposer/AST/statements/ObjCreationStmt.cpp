#include "ObjCreationStmt.h"

#include "class/ClassNode.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "../../errorHandling/classErrors/InstantiateAbstractClass.h"
#include "errorHandling/classErrors/ClassDosentExisit.h"
#include "symbols/Type/ClassType.h"
#include "other/SymbolTable.h"

ObjCreationStmt::ObjCreationStmt(const Token& token,
                                 const ClassNode* classNode, const bool hasStartingValue, std::unique_ptr<ConstractorCallStmt> startingValue,
                                 const Var& var)
: VarDeclStmt(token, hasStartingValue, std::move(startingValue), var),
      classNode(classNode)
{
}

void ObjCreationStmt::analyze() const
{
    if (symTable == nullptr) return;

    const ClassNode* target = classNode;
    if (target == nullptr)
    {
        target = symTable->getClass(var.getType()->getFQN());
        if (target == nullptr) symTable->addError(std::make_unique<ClassDosentExisit>(token, var.getType()->toString()));
        const_cast<ObjCreationStmt*>(this)->classNode = target;
    }

    if (startingValue != nullptr)
    {
        if (auto* ctorCall = dynamic_cast<ConstractorCallStmt*>(const_cast<Expr*>(startingValue.get())))
        {
            ctorCall->setTargetClass(target);
            ctorCall->setSymbolTable(symTable);
            ctorCall->setScope(scope);
            ctorCall->setClassNode(currClass);
            ctorCall->analyze();
            
            if (target->isAbstract())
            {
                symTable->addError(std::make_unique<InstantiateAbstractClass>(token));
            }
        }
    }
    
    VarDeclStmt::analyze();
}

std::string ObjCreationStmt::translateToCpp() const
{
    std::ostringstream oss;
    if (classNode == nullptr) symTable->addError(std::make_unique<HowDidYouGetHere>(token));

    const std::string className = translateFQNtoString(classNode->getClass().getClassName());
    const std::string classSafePtr = "SafePtr<" + className + ">";

    oss << getTabs() << classSafePtr << " " << translateFQNtoString(var.getName()) << " = " << classSafePtr << "(";

    if (hasStartingValue && startingValue != nullptr) oss  << startingValue->translateToCpp();
    else oss << className << "()";

    oss << ");";

    return oss.str();
}
