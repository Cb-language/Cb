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
    const std::string name;
    std::vector<Field> fields;
    std::vector<Method> methods;
    std::vector<Ctor> ctors;

    bool isInheriting = false;
    AccessType inheritingPublic;
    const std::string inheritingName;

    std::string generateToString() const;
    std::string generateEquals() const;

public:
    ClassDeclStmt(const Token& token, IFuncDeclStmt* funcDecl, const std::string& name, std::vector<Field>& fields,
        std::vector<Method>& methods, std::vector<Ctor>& ctors, const bool isInheriting, AccessType inheritingPublic, const std::string& inheritingName, ClassDeclStmt* classDecl = nullptr);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::string translateToH() const override;
    bool getHasEmptyCtor() const;
};
