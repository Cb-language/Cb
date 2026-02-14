#pragma once
#include "IType.h"
#include "class/ClassNode.h"

class ClassType : public IType
{
private:
    const ClassNode* c;

public:
    explicit ClassType(const ClassNode* c);
    ~ClassType() override;

    bool operator==(const IType& other) const override;
    bool operator!=(const IType& other) const override;
    bool operator==(const std::wstring& other) const override;
    bool operator!=(const std::wstring& other) const override;
    bool isNumberable() const override;
    bool isStringable() const override;
    bool isPrimitive() const override;
    std::wstring getType() const override;
    std::string translateTypeToCpp() const override;
    std::unique_ptr<IType> copy() const override;
};
