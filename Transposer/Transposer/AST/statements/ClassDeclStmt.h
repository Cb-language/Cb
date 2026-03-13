#pragma once
#include "ConstructorDeclStmt.h"
#include "FuncDeclStmt.h"
#include "VarDeclStmt.h"
#include "AST/abstract/Statement.h"
#include "class/AccessType.h"

typedef std::pair<AccessType, std::unique_ptr<VarDeclStmt>> Field;
typedef std::pair<AccessType, std::unique_ptr<FuncDeclStmt>> Method;
typedef std::pair<AccessType, std::unique_ptr<ConstructorDeclStmt>> Ctor;

class ClassDeclStmt : public Stmt
{
private:
    bool hasEmptyCtor = false;
    FQN name;
    std::vector<Field> fields;
    std::vector<Method> methods;
    std::vector<Ctor> ctors;

    FQN parentName;

    std::string generateToString() const;
    std::string generateEquals() const;

public:
    ClassDeclStmt(const Token& token, const FQN& name, std::vector<Field>& fields,
        std::vector<Method>& methods, std::vector<Ctor>& ctors, const FQN& parentName);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::string translateToH() const override;
    bool getHasEmptyCtor() const;

    void setClassDetails(std::vector<Field> fields, std::vector<Method> methods, std::vector<Ctor> ctors);

    const FQN& getName() const;
    const FQN& getParentName() const;
    const std::vector<Field>& getFields() const;
    const std::vector<Method>& getMethods() const;
    const std::vector<Ctor>& getCtors() const;
};
