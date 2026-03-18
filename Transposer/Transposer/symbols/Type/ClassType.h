#pragma once
#include "IType.h"
#include "class/ClassNode.h"
#include "parser/FQN.h"
#include "class/ClassTree.h"

class ClassType : public IType
{
private:
    const FQN name;
    const ClassNode* c;

    static ClassTree& classTree;
public:
    explicit ClassType(const FQN& name);
    ~ClassType() override;

    bool operator==(const IType& other) const override;
    bool operator!=(const IType& other) const override;

    bool isNumberable() const override;
    bool isStringable() const override;
    bool isPrimitive() const override;
    const ClassNode* getClassNode() const;
    void setClassNode(const ClassNode& node);
    const FQN& getName() const;

    std::string translateTypeToCpp() const override;
    std::unique_ptr<IType> copy() const override;
    std::string toString() const override;

    FQN getFQN() const override;

};
