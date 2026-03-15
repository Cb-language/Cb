#include "ObjCreationPolyStmt.h"

#include "class/ClassNode.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "../../errorHandling/classErrors/InstantiateAbstractClass.h"
#include "errorHandling/classErrors/ClassDosentExisit.h"
#include "symbols/Type/ClassType.h"
#include "other/SymbolTable.h"

ObjCreationPolyStmt::ObjCreationPolyStmt(const Token& token, const ClassNode* classNode, const bool hasStartingValue,
    std::unique_ptr<ConstractorCallStmt> startingValue, const Var& var, const FQN& cname)
    : ObjectCreationStmt(token, hasStartingValue, std::move(startingValue), var),
      classNode(classNode)
{
    if (cname.empty())
    {
        ctorName = var.getName();
    }
    else
    {
        for (const auto& n : cname)
        {
            ctorName.emplace_back(n);
        }
    }
}

void ObjCreationPolyStmt::analyze() const
{
    if (symTable == nullptr) return;

    const ClassNode* target = classNode;
    if (target == nullptr)
    {
        auto searchingName = var.getType()->getFQN();
        target = symTable->getClass(searchingName);
        if (target == nullptr) symTable->addError(std::make_unique<ClassDosentExisit>(token, translateFQNtoString(searchingName)));

        searchingName = ctorName.empty() ? var.getType()->getFQN() : ctorName;
        target = symTable->getClass(searchingName);
        if (target == nullptr) symTable->addError(std::make_unique<ClassDosentExisit>(token, translateFQNtoString(searchingName)));
        const_cast<ObjCreationPolyStmt*>(this)->classNode = target;
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

std::string ObjCreationPolyStmt::translateToCpp() const
{
    std::ostringstream oss;
    if (classNode == nullptr) throw HowDidYouGetHere(token);

    const std::string className = translateFQNtoString(var.getType()->getFQN());
    const std::string classSafePtr = "SafePtr<" + className + ">";
    const std::string ctorCall = translateFQNtoString(classNode->getClass().getClassName());
    const std::string ctorSafePtr = "SafePtr<" + ctorCall + ">";

    oss << getTabs() << classSafePtr << " " << translateFQNtoString(var.getName()) << " = " << classSafePtr << "(";

    if (hasStartingValue && startingValue != nullptr) oss  << startingValue->translateToCpp();
    else oss << ctorCall << "()";

    oss << ");";

    return oss.str();
}
