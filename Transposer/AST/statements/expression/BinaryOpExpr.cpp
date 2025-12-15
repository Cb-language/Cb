#include "BinaryOpExpr.h"

#include <sstream>

BinaryOpExpr::BinaryOpExpr(Scope* scope, FuncDeclStmt* funcDecl, const std::wstring& op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right, const bool hasParens)
: Expr(scope, funcDecl, hasParens), op(op), left(std::move(left)), right(std::move(right))
{
}

std::unique_ptr<IType> BinaryOpExpr::getType() const
{
    auto leftType = left->getType();
    auto rightType = right->getType();

    // Comparison operators
    if (op == L"==" || op == L"!=" ||
        op == L"<"  || op == L">"  ||
        op == L"<=" || op == L">=")
    {
        return std::make_unique<Type>(L"mute");
    }

    // Logical operators
    if (op == L"&&" || op == L"||")
    {
        return std::make_unique<Type>(L"mute");
    }

    // the // op
    if (op == L"//")
    {
        return std::make_unique<Type>(L"freq");
    }

    // Arithmetic operators
    if (op == L"+")
    {
        if (leftType->getType() == L"bar" || rightType->getType() == L"bar")
            return std::make_unique<Type>(L"bar");
        if (*leftType == L"freq" || *rightType == L"freq")
            return std::make_unique<Type>(L"freq");
        if (*leftType == L"note" || *rightType == L"note")
            return std::make_unique<Type>(L"note");
    }

    if (op == L"-" ||
        op == L"*"  || op == L"/" ||
        op == L"%")
    {
        // If both types are identical → return that type
        if (*leftType == *rightType)
            return leftType;

        // Otherwise: promote
        if (*leftType == L"freq" || *rightType == L"freq")
            return std::make_unique<Type>(L"freq");
        if (*leftType == L"note" || *rightType == L"note")
            return std::make_unique<Type>(L"note");

        // Otherwise int
        return std::make_unique<Type>(L"degree");
    }

    return std::make_unique<Type>(L"fermata");
}

bool BinaryOpExpr::isLegal() const
{
    auto leftType = left->getType();
    auto rightType = right->getType();

    // Comparison operators
    if (op == L"==" || op == L"!=" ||
        op == L"<"  || op == L">"  ||
        op == L"<=" || op == L">=")
    {
        return *leftType == *rightType;
    }

    // Logical operators
    if (op == L"&&" || op == L"||")
    {
        return *leftType == *rightType;
    }

    // the + op
    if (op == L"+")
    {
        return (leftType->isNumberable() && rightType->isNumberable()) || (leftType->isStringable() && rightType->isStringable());
    }

    // Arithmetic operators
    if (op == L"//"  || op == L"-" ||
        op == L"*"  || op == L"/" ||
        op == L"%")
    {
        return leftType->isNumberable() && rightType->isNumberable();
    }

    return false;
}

std::string BinaryOpExpr::translateToCpp() const
{
    const std::string leftStr = left->translateToCpp();
    const std::string rightStr = right->translateToCpp();
    const std::string leftType = left->getType()->translateTypeToCpp();
    const std::string rightType = right->getType()->translateTypeToCpp();
    const std::string opStr = Utils::wstrToStr(op);
    std::ostringstream oss;

    if (op == L"//")
    {
        std::string leftStrClean = leftStr;
        std::string rightStrClean = rightStr;
        
        if (leftStr.starts_with("(") && leftStr.ends_with(")"))
        {
            leftStrClean = leftStrClean.substr(1, leftStrClean.size() - 2);
        }
        
        if (rightStr.starts_with("(") && rightStr.ends_with(")"))
        {
            rightStrClean = rightStrClean.substr(1, rightStrClean.size() - 2);
        }
        
        oss << "static_cast<double>(" << leftStrClean << ")"
        << " / static_cast<double>(" << rightStrClean << ")";
    }
    else if (opStr == "+" && leftType == "bar" && rightType != "bar")
    {
        oss << leftStr << " " << opStr << " std::to_string(" << rightStr << ")";
    }
    else if (opStr == "+" && leftType != "bar" && rightType == "bar")
    {
        oss << "std::to_string(" << leftStr << ") " << opStr << " " << rightStr;
    }
    else
    {
        oss << leftStr << " " << opStr << " " << rightStr;
    }

    if (hasParens)
    {
        return "(" + oss.str() + ")";
    }

    return oss.str();
}

int BinaryOpExpr::getPrecedence(const std::wstring& op)
{
    // Multiplicative
    if (op == L"*" || op == L"/" || op == L"%" || op == L"//")
        return 3;

    // Additive
    if (op == L"+" || op == L"-")
        return 2;

    // Relational
    if (op == L"<"  || op == L">" ||
        op == L"<=" || op == L">=")
        return 1;

    // Equality
    if (op == L"==" || op == L"!=")
        return 1;

    return 0;
}
