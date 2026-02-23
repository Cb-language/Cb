#include "ClassNode.h"

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
    if (parent != nullptr && parent->isLegal(field, curr)) return true;

    for (const auto& [accessType, f] : c.getFields())
    {
        if (curr != this && !accessType) continue;
        if (f == field) return true;
    }
    return false;
}

bool ClassNode::isLegal(const Func& method, const ClassNode* curr) const
{
    if (parent != nullptr && parent->isLegal(method, curr)) return true;

    for (const auto& [accessType, m] : c.getMethods())
    {
        if (curr != this && !accessType) continue;
        if (m == method) return true;
    }
    return false;
}

bool ClassNode::isLegal(const Constractor& ctor, const ClassNode* curr) const
{
    if (parent != nullptr && parent->isLegal(ctor, curr)) return true;

    for (const auto& [accessType, ct] : c.getConstractors())
    {
        if (curr != this && !accessType) continue;
        if (ct == ctor) return true;
    }
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

const Class& ClassNode::getClass() const
{
    return c;
}
