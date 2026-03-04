#include "ClassTree.h"

std::vector<std::unique_ptr<ClassNode>> ClassTree::classes;
ClassNode* ClassTree::root = nullptr;

ClassTree& ClassTree::instance()
{
    static ClassTree instance;
    return instance;
}

void ClassTree::addClass(const Class& c, ClassNode* p)
{
    std::unique_ptr<ClassNode> temp;
    if (p == nullptr)
    {
        if (root == nullptr) temp = std::make_unique<ClassNode>(c);
        else temp = std::make_unique<ClassNode>(c, root);
    }
    else
    {
        temp = std::make_unique<ClassNode>(c, p);
        p->addChild(temp.get());
    }

    classes.push_back(std::move(temp));
}

ClassNode* ClassTree::find(const std::wstring& name)
{
    for (const auto& cl : classes)
    {
        if (name == cl->getClass().getClassName())
        {
            return cl.get();
        }
    }
    return nullptr;
}

void ClassTree::init()
{
    const std::vector<std::pair<AccessType, Func>> methods =
    {
        {
            PUBLIC,
            Func(
                std::make_unique<Type>(L"bar"),
                L"toString",
                std::vector<Var>(),
                VirtualType::None)
            }
    };
    constexpr std::vector<std::pair<AccessType, Var>> args;
    const std::vector<std::pair<AccessType, Constractor>> ctors =
    {
        {
            PUBLIC,
            Constractor(std::vector<Var>{}, L"Object")
        }
    };

    Class obj = Class(L"Object", methods, args, ctors);

    instance().addClass(obj, nullptr);

    root = classes.back().get();
}

void ClassTree::destroy()
{
    classes.clear();
    root = nullptr;
}
