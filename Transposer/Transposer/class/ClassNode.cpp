#include "ClassNode.h"

#include <ranges>

ClassNode::ClassNode(const Class& c, ClassNode* parent) : parent(parent), c(c.copy())
{
}

const ClassNode* ClassNode::getParent() const
{
    return parent;
}

const std::vector<ClassNode*>& ClassNode::getChildren() const
{
    return children;
}

bool ClassNode::isLegal(const Var& field, const ClassNode* curr) const
{
    for (const auto& [accessType, f] : c.getFields())
    {
        if (f == field)
        {
            if (curr == this) return true;
            if (accessType == PUBLIC) return true;
            if (accessType == PROTECTED && curr != nullptr && curr->isDescendantOf(this)) return true;
            return false;
        }
    }

    if (parent != nullptr && parent->isLegal(field, curr)) return true;

    return false;
}

bool ClassNode::isLegal(const Func& method, const ClassNode* curr) const
{
    for (const auto& [accessType, m] : c.getMethods())
    {
        if (m == method)
        {
            if (curr == this) return true;
            if (accessType == PUBLIC) return true;
            if (accessType == PROTECTED && curr != nullptr && curr->isDescendantOf(this)) return true;
            return false;
        }
    }

    if (parent != nullptr && parent->isLegal(method, curr)) return true;

    return false;
}

bool ClassNode::isLegal(const Constractor& ctor, const ClassNode* curr) const
{
    for (const auto& [accessType, ct] : c.getConstractors())
    {
        if (ct == ctor)
        {
            if (curr == this) return true;
            if (accessType == PUBLIC) return true;
            if (accessType == PROTECTED && curr != nullptr && curr->isDescendantOf(this)) return true;
            return false;
        }
    }

    if (parent != nullptr && parent->isLegal(ctor, curr)) return true;

    return false;
}

void ClassNode::add(const AccessType accessType, const Var& field)
{
    c.addField(accessType, field);
}

void ClassNode::add(const AccessType accessType, const Func& method)
{
    c.addMethod(accessType, method);
}

void ClassNode::add(const AccessType accessType, const Constractor& ctor)
{
    c.addConstractor(accessType, ctor);
}

void ClassNode::addChild(ClassNode* cl)
{
    children.emplace_back(cl);
}

bool ClassNode::isDescendantOf(const ClassNode* other) const
{
    if (this == other) return true;
    if (parent == nullptr) return false;
    return parent->isDescendantOf(other);
}

const Var* ClassNode::findField(const std::wstring& name) const
{
    for (const auto& field : c.getFields() | std::views::values)
    {
        if (field.getName() == name) return &field;
    }
    if (parent != nullptr) return parent->findField(name);
    return nullptr;
}

const Func* ClassNode::findMethod(const std::wstring& name) const
{
    for (const auto& method : c.getMethods() | std::views::values)
    {
        if (method.getFuncName() == name) return &method;
    }
    if (parent != nullptr) return parent->findMethod(name);
    return nullptr;
}

const Class& ClassNode::getClass() const
{
    return c;
}
