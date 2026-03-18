#pragma once

#include "../Transposer/symbols/Class.h"
#include "ConstractorCallStmt.h"
#include "AST/abstract/ObjectCreationStmt.h"

class ObjCreationPolyStmt : public ObjectCreationStmt
{
private:
    const ClassNode* classNode;
    FQN ctorName;
public:
    ObjCreationPolyStmt(const Token& token, const ClassNode* classNode,
        bool hasStartingValue, std::unique_ptr<ConstractorCallStmt> startingValue, const Var &var, const FQN &cname = {});

    void analyze() const override;
    std::string translateToCpp() const override;
};
