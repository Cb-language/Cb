#include "ClassNode.h"

ClassNode::ClassNode(const Class& c, ClassNode* parent) : parent(parent), c(c.copy())
{
}

bool ClassNode::isLegal(const Var& field, const ClassNode* curr) const
{
    if (parent != nullptr && parent->isLegal(field, curr)) return true;

    for (const auto& [isPublic, f] : c.getFields())
    {
        if (curr != this && !isPublic) continue;
        if (f == field) return true;
    }
    return false;
}

bool ClassNode::isLegal(const Func& method, const ClassNode* curr) const
{
    if (parent != nullptr && parent->isLegal(method, curr)) return true;

    for (const auto& [isPublic, m] : c.getMethods())
    {
        if (curr != this && !isPublic) continue;
        if (m == method) return true;
    }
    return false;
}

bool ClassNode::isLegal(const Constractor& ctor, const ClassNode* curr) const
{
    if (parent != nullptr && parent->isLegal(ctor, curr)) return true;

    for (const auto& [isPublic, ct] : c.getConstractors())
    {
        if (curr != this && !isPublic) continue;
        if (ct == ctor) return true;
    }
    return false;
}

void ClassNode::add(const bool isPublic, const Var& field)
{
    c.addField(isPublic, field);
}

void ClassNode::add(const bool isPublic, const Func& method)
{
    c.addMethod(isPublic, method);
}

void ClassNode::add(const bool isPublic, const Constractor& ctor)
{
    c.addConstractor(isPublic, ctor);
}

void ClassNode::addChild(const Class& cl)
{
    children.emplace_back(std::make_unique<ClassNode>(cl, this));
}

const Class& ClassNode::getClass() const
{
    return c;
}
