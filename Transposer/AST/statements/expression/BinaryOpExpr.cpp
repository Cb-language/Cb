#include "BinaryOpExpr.h"

#include <sstream>

BinaryOpExpr::BinaryOpExpr(Scope* scope, FuncDeclStmt* funcDecl, const std::wstring& op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right, const bool hasParens)
: Expr(scope, funcDecl, hasParens), op(op), left(std::move(left)), right(std::move(right))
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
    if (op == L"+")
    {
        if (leftType.getType() == L"bar" || rightType.getType() == L"bar")
            return Type(L"bar");
        if (leftType == L"freq" || rightType == L"freq")
            return Type(L"freq");
        if (leftType == L"note" || rightType == L"note")
            return Type(L"note");
    }

    if (op == L"-" ||
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

    return Type(L"fermata");
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
    const std::string leftStr = left->translateToCpp();
    const std::string rightStr = right->translateToCpp();
    const std::wstring leftType = left.get()->getType().getType();
    const std::wstring rightType = right.get()->getType().getType();
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
    else if (opStr == "+" && leftType == L"bar" && rightType != L"bar")
    {
        oss << leftStr << " " << opStr << " std::to_string(" << rightStr << ")";
    }
    else if (opStr == "+" && leftType != L"bar" && rightType == L"bar")
    {
        oss << "std::to_string(" <<leftStr << ") " << opStr << " " << rightStr;
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
