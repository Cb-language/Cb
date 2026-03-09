#pragma once
#include "IType.h"
#include "class/ClassNode.h"

class ClassType : public IType
{
private:
    const std::string name;
    const ClassNode* c;

public:
    explicit ClassType(const std::string& name);
    ~ClassType() override;

    bool operator==(const IType& other) const override;
    bool operator!=(const IType& other) const override;

    bool isNumberable() const override;
    bool isStringable() const override;
    bool isPrimitive() const override;
    const ClassNode* getClassNode() const;
    void setClassNode(const ClassNode& node);

    std::string translateTypeToCpp() const override;
    std::unique_ptr<IType> copy() const override;
    std::string toString() const override;

};
