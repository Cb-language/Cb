#pragma once
#include "AST/statements/ClassDeclStmt.h"
#include "symbols/Class.h"

class ClassNode
{
private:
    ClassNode* parent;
    Class c;
    std::vector<std::unique_ptr<ClassNode>> children;

public:
    explicit ClassNode(const Class& c, ClassNode* parent = nullptr);
    ~ClassNode() = default;

    bool isLegal(const Var& field, const ClassNode* curr) const;
    bool isLegal(const Func& method, const ClassNode* curr) const;
    bool isLegal(const Constractor& ctor, const ClassNode* curr) const;

    void add(const AccessType accessType, const Var& field);
    void add(const AccessType accessType, const Func& method);
    void add(const AccessType accessType, const Constractor& ctor);

    void addChild(const Class& cl);

    const Class& getClass() const;
};