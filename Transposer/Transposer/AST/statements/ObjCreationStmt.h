#pragma once

#include "VarDeclStmt.h"
#include "../Transposer/symbols/Class.h"
#include "expression/ConstractorCallStmt.h"

class ObjCreationStmt : public VarDeclStmt
{
private:
    const ClassNode* classNode;
public:
    ObjCreationStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass, const ClassNode* classNode,
        bool hasStartingValue, std::unique_ptr<ConstractorCallStmt> startingValue, const Var &var);

    std::string translateToCpp() const override;
};
