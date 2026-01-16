#pragma once
#include <string>

class IType
{
protected:
    const std::wstring type;
    
public:
    explicit IType(const std::wstring& type) : type(type) {};
    explicit IType(IType* other) : type(other->getType()) {};
    virtual ~IType() = default;
    virtual bool operator==(const IType& other) const = 0;
    virtual bool operator!=(const IType& other) const = 0;
    virtual bool operator==(const std::wstring &other) const = 0;
    virtual bool operator!=(const std::wstring &other) const = 0;

    virtual bool isNumberable() const = 0;
    virtual bool isStringable() const = 0;

    virtual bool isPrimitive() const = 0;

    virtual std::wstring getType() const = 0;

    virtual std::string translateTypeToCpp() const = 0;

    virtual std::unique_ptr<IType> copy() const = 0;

    virtual unsigned int getArrLevel() const {return 0;} // 0 - non array

    virtual std::unique_ptr<IType> getArrType() const {return nullptr;}
};
