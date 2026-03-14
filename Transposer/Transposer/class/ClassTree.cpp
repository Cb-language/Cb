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

ClassNode* ClassTree::find(const FQN& name)
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
    std::vector<std::string> s = {"toString"};
    const std::vector<std::pair<AccessType, Func>> methods =
    {
        {
            PUBLIC,
            Func(
                std::make_unique<PrimitiveType>(Primitive::TYPE_BAR),
                s,
                std::vector<Var>(),
                VirtualType::NONE)
            }
    };
    s.pop_back();
    s.push_back("Object");
    constexpr std::vector<std::pair<AccessType, Var>> args;
    const std::vector<std::pair<AccessType, Constractor>> ctors =
    {
        {
            PUBLIC,
            Constractor(std::vector<Var>{}, s)
        }
    };

    const auto obj = Class(s, methods, args, ctors);

    instance().addClass(obj, nullptr);

    root = classes.back().get();
}

void ClassTree::destroy()
{
    classes.clear();
    root = nullptr;
}
