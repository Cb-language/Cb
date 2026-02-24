#pragma once
#include "AST/statements/ClassDeclStmt.h"
#include "symbols/Class.h"

class ClassNode
{
private:
    ClassNode* parent;
    Class c;
    std::vector<ClassNode*> children;

public:
    explicit ClassNode(const Class& c, ClassNode* parent = nullptr);
    ~ClassNode() = default;

    const ClassNode* getParent() const;
    const std::vector<ClassNode*>& getChildren() const;

    bool isLegal(const Var& field, const ClassNode* curr) const;
    bool isLegal(const Func& method, const ClassNode* curr) const;
    bool isLegal(const Constractor& ctor, const ClassNode* curr) const;

    void add(const AccessType accessType, const Var& field);
    void add(const AccessType accessType, const Func& method);
    void add(const AccessType accessType, const Constractor& ctor);

    void addChild(ClassNode* cl);

    bool isDescendantOf(const ClassNode* other) const;

    const Var* findField(const std::wstring& name) const;
    const Func* findMethod(const std::wstring& name) const;

    const Class& getClass() const;
};