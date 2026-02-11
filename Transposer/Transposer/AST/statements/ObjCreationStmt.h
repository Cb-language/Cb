#pragma once

#include "AST/abstract/Statement.h"
#include "../Transposer/symbols/Class.h"

class ObjCreationStmt : public Stmt
{
private:
    bool hasCtor = false;
    Class _class;
    const std::wstring name;
public:
    ObjCreationStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const std::optional<Class>& _class, const std::wstring& name);

    void analyze() const override;
    std::string translateToCpp() const override;
};
