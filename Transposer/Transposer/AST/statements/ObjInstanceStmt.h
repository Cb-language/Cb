#pragma once
#include "AST/abstract/Statement.h"

class ObjInstanceStmt : public Stmt
{
private:
    const ClassNode* classNode;
    bool isFuncCall;
    std::wstring fieldName;
public:
    ObjInstanceStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, ClassNode* currClass, const ClassNode* classNode, const bool& isFuncCall, const std::wstring& fieldName);

    void analyze() const override;
    std::string translateToCpp() const override;
};
