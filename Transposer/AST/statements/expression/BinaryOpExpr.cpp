#include "BinaryOpExpr.h"

BinaryOpExpr::BinaryOpExpr(const std::wstring& op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right) : op(op), left(std::move(left)), right(std::move(right))
{
}

Type BinaryOpExpr::getType() const
{
    const Type leftType = left->getType();
    const Type rightType = right->getType();

    // Comparison operators
    if (op == L"==" || op == L"!=" ||
        op == L"<"  || op == L">"  ||
        op == L"<=" || op == L">=")
    {
        return Type(L"mute");
    }

    // Logical operators
    if (op == L"&&" || op == L"||")
    {
        return Type(L"mute");
    }

    // the // op
    if (op == L"//")
    {
        return Type(L"freq");
    }

    // Arithmetic operators
    if (op == L"+"  || op == L"-" ||
        op == L"*"  || op == L"/" ||
        op == L"%")
    {
        // If both types are identical → return that type
        if (leftType == rightType)
            return leftType;

        // Otherwise: promote
        if (leftType == L"freq" || rightType == L"freq")
            return Type(L"freq");
        if (leftType == L"note" || rightType == L"note")
            return Type(L"note");

        // Otherwise int
        return Type(L"degree");
    }

    return Type(L"UNKNOWN");
}

bool BinaryOpExpr::isLegal() const
{
    const Type leftType = left->getType();
    const Type rightType = right->getType();

    // Comparison operators
    if (op == L"==" || op == L"!=" ||
        op == L"<"  || op == L">"  ||
        op == L"<=" || op == L">=")
    {
        return leftType == rightType;
    }

    // Logical operators
    if (op == L"&&" || op == L"||")
    {
        return leftType == rightType;
    }

    // the + op
    if (op == L"+")
    {
        return (leftType.isNumberable() && rightType.isNumberable()) || (leftType.isStringable() && rightType.isStringable());
    }

    // Arithmetic operators
    if (op == L"//"  || op == L"-" ||
        op == L"*"  || op == L"/" ||
        op == L"%")
    {
        return leftType.isNumberable() && rightType.isNumberable();
    }

    return false;
}

std::string BinaryOpExpr::translateToCpp() const
{
    return left->translateToCpp() + " " + Utils::wstrToStr(op) + " " + right->translateToCpp();
}
