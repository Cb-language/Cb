#pragma once
#include "ConstractorDeclStmt.h"
#include "FuncDeclStmt.h"
#include "VarDeclStmt.h"
#include "AST/abstract/Statement.h"

enum accessType
{
    PUBLIC = 0,
    PROTECTED = 1,
    PRIVATE = 2
};

typedef std::pair<accessType, std::unique_ptr<VarDeclStmt>> Field;
typedef std::pair<accessType, std::unique_ptr<FuncDeclStmt>> Method;
typedef std::pair<accessType, std::unique_ptr<ConstractorDeclStmt>> Ctor;

class ClassDeclStmt : public Stmt
{
private:
    const std::wstring name;
    std::vector<Field> fields;
    std::vector<Method> methods;
    std::vector<Ctor> ctors;

    bool isInhereting = false;
    const std::wstring inheritingPublic;
    const std::wstring inheretingName;

public:
    ClassDeclStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass, const std::wstring& name, std::vector<Field>& fields,
        std::vector<Method>& methods, std::vector<Ctor>& ctors, const bool isInhereting, const std::wstring& inheritingPublic, const std::wstring& inheretingName);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::string translateToH() const override;
};
