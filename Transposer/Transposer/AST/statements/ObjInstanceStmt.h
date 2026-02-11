#pragma once
#include "AST/abstract/Statement.h"

class ObjInstanceStmt : public Stmt
{
private:
    std::unique_ptr<Class> _class;
    bool isFuncCall;
    std::wstring fieldName;
public:
    ObjInstanceStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, std::unique_ptr<Class>& _class, const bool& isFuncCall, const std::wstring& fieldName);

    void analyze() const override;
    std::string translateToCpp() const override;
};
