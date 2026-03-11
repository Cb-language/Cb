#pragma once

#include "VarDeclStmt.h"
#include "../Transposer/symbols/Class.h"
#include "ConstractorCallStmt.h"

class ObjCreationStmt : public VarDeclStmt
{
private:
    const ClassNode* classNode;
public:
    ObjCreationStmt(const Token& token, const ClassNode* classNode,
        bool hasStartingValue, std::unique_ptr<ConstractorCallStmt> startingValue, const Var &var);

    void analyze() const override;
    std::string translateToCpp() const override;
};
