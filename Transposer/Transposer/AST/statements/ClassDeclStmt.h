#pragma once
#include "ConstractorDeclStmt.h"
#include "FuncDeclStmt.h"
#include "VarDeclStmt.h"
#include "AST/abstract/Statement.h"
#include "class/AccessType.h"

typedef std::pair<AccessType, std::unique_ptr<VarDeclStmt>> Field;
typedef std::pair<AccessType, std::unique_ptr<FuncDeclStmt>> Method;
typedef std::pair<AccessType, std::unique_ptr<ConstractorDeclStmt>> Ctor;

class ClassDeclStmt : public Stmt
{
private:
    bool hasEmptyCtor = false;
    const std::wstring name;
    std::vector<Field> fields;
    std::vector<Method> methods;
    std::vector<Ctor> ctors;

    bool isInheriting = false;
    const std::wstring inheritingPublic;
    const std::wstring inheritingName;

public:
    ClassDeclStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass, const std::wstring& name, std::vector<Field>& fields,
        std::vector<Method>& methods, std::vector<Ctor>& ctors, const bool isInheriting, const std::wstring& inheritingPublic, const std::wstring& inheritingName);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::string translateToH() const override;
    bool getHasEmptyCtor() const;
};
