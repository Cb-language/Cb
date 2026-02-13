#include "BinaryOpExpr.h"

#include <sstream>

#include "errorHandling/semanticErrors/IllegalOpOnType.h"

BinaryOpExpr::BinaryOpExpr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass,
    const std::wstring& op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right, const bool hasParens)
        : Expr(token, scope, funcDecl, currClass, hasParens), op(op), left(std::move(left)), right(std::move(right))
{
}

std::unique_ptr<IType> BinaryOpExpr::getType() const
{
    if (left == nullptr)
    {
        return right->getType()->copy();
    }

    auto leftType = left->getType()->copy();
    auto rightType = right->getType()->copy();

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

void BinaryOpExpr::analyze() const
{
    auto leftType = left != nullptr ? left->getType() : nullptr;
    auto rightType = right->getType();

    // Comparison operators
    if (op == L"==" || op == L"!=" ||
        op == L"<"  || op == L">"  ||
        op == L"<=" || op == L">=")
    {
        if (*leftType != *rightType)
        {
            throw IllegalOpOnType(token, leftType->toString(), rightType->toString(), Utils::wstrToStr(op));
        }
    }

    // Logical operators
    if (op == L"&&" || op == L"||")
    {
        if (*leftType != *rightType)
        {
            throw IllegalOpOnType(token, leftType->toString(), rightType->toString(), Utils::wstrToStr(op));
        }
    }

    if (left == nullptr)
    {
        if (!rightType->isNumberable())
        {
            throw IllegalOpOnType(token, rightType->toString());
        }
    }

    // the + op
    if (op == L"+")
    {
        if (!(leftType->isNumberable() && rightType->isNumberable()) || (leftType->isStringable() && rightType->isStringable()))
        {
            throw IllegalOpOnType(token, leftType->toString(), rightType->toString(), "+");
        }
    }

    // Arithmetic operators
    if (op == L"//"  || op == L"-" ||
        op == L"*"  || op == L"/" ||
        op == L"%")
    {
        if (!(leftType->isNumberable() && rightType->isNumberable()))
        {
            throw IllegalOpOnType(token, leftType->toString(), rightType->toString(), Utils::wstrToStr(op));
        }
    }
}

std::string BinaryOpExpr::translateToCpp() const
{
    const bool isLeftNull = left == nullptr;
    const std::string leftStr = !isLeftNull ? left->translateToCpp() : "";
    const std::string rightStr = right->translateToCpp();
    const std::wstring leftType = !isLeftNull ? left->getType()->getType() : L"degree";
    const std::wstring rightType = right->getType()->getType();
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

        if (isLeftNull)
        {
            oss << "1 / static_cast<double>(" << rightStrClean << ")";
        }
        else
        {
            oss << "static_cast<double>(" << leftStrClean << ")"
           << " / static_cast<double>(" << rightStrClean << ")";
        }
    }
    else if (opStr == "+" && leftType == L"bar" && rightType != L"bar")
    {
        oss << leftStr << " " << opStr << " std::to_string(" << rightStr << ")";
    }
    else if (opStr == "+" && leftType != L"bar" && rightType == L"bar")
    {
        oss << "std::to_string(" << leftStr << ") " << opStr << " " << rightStr;
    }
    else if (isLeftNull && (opStr == "*" || opStr == "/"))
    {
        oss << "1" << opStr << rightStr;
    }
    else if (isLeftNull && (opStr == "-" || opStr == "+"))
    {
        const std::string str = opStr == "-" ? "-" : "";
        oss << str << rightStr;
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
