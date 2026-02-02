#pragma once
#include "VarDeclStmt.h"
#include "AST/abstract/Statement.h"

typedef std::pair<bool, std::unique_ptr<VarDeclStmt>> Field;
typedef std::pair<bool, std::unique_ptr<FuncDeclStmt>> Method;

class ClassDeclStmt : public Stmt
{
private:
    const std::wstring name;
    std::vector<Field> fields;
    std::vector<Method> methods;

public:
    ClassDeclStmt(const Token& token, Scope* scope, FuncDeclStmt* funcDecl, const std::wstring& name, std::vector<Field>& fields, std::vector<Method>& methods);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::string translateToH() const override;
};
