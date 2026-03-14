#pragma once

#include "VarDeclStmt.h"
#include "../Transposer/symbols/Class.h"
#include "ConstractorCallStmt.h"

class ObjCreationStmt : public VarDeclStmt
{
private:
    const ClassNode* classNode;
    FQN ctorName;
public:
    ObjCreationStmt(const Token& token, const ClassNode* classNode,
        bool hasStartingValue, std::unique_ptr<ConstractorCallStmt> startingValue, const Var &var, const FQN &cname = {});

    void analyze() const override;
    std::string translateToCpp() const override;
};
