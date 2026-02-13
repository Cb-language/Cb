#pragma once
#include "ConstractorDeclStmt.h"
#include "FuncDeclStmt.h"
#include "VarDeclStmt.h"
#include "AST/abstract/Statement.h"

typedef std::pair<bool, std::unique_ptr<VarDeclStmt>> Field;
typedef std::pair<bool, std::unique_ptr<FuncDeclStmt>> Method;
typedef std::pair<bool, std::unique_ptr<ConstractorDeclStmt>> Ctor;

class ClassDeclStmt : public Stmt
{
private:
    const std::wstring name;
    std::vector<Field> fields;
    std::vector<Method> methods;
    std::vector<Ctor> ctors;

public:
    ClassDeclStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, ClassNode* currClass, const std::wstring& name, std::vector<Field>& fields,
        std::vector<Method>& methods, std::vector<Ctor>& ctors);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::string translateToH() const override;
};
