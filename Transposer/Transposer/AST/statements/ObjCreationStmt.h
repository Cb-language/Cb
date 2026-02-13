#pragma once

#include "AST/abstract/Statement.h"
#include "../Transposer/symbols/Class.h"

class ObjCreationStmt : public Stmt
{
private:
    bool hasCtor = false;
    const ClassNode* classNode;
    const std::wstring name;
public:
    ObjCreationStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass, const ClassNode* classNode, const std::wstring& name);

    void analyze() const override;
    std::string translateToCpp() const override;
};
