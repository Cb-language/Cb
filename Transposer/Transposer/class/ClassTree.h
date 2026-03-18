#pragma once
#include "ClassNode.h"

class ClassTree
{
private:
    static std::vector<std::unique_ptr<ClassNode>> classes;
    static ClassNode* root;

    ClassTree() = default;
    ~ClassTree() = default;

public:
    ClassTree(const ClassTree&) = delete;
    ClassTree& operator=(const ClassTree&) = delete;

    ClassTree(ClassTree&&) = delete;
    ClassTree& operator=(ClassTree&&) = delete;

    static ClassTree& instance();

    void addClass(const Class& c, ClassNode* p);
    ClassNode* find(const FQN& name);

    static void init();
    static void destroy();
};
