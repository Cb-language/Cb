#include "BinaryOpExpr.h"

#include <sstream>

#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "other/SymbolTable.h"

BinaryOpExpr::BinaryOpExpr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass,
                           const std::string& op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right, const bool hasParens)
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
    if (op == "==" || op == "!=" ||
        op == "<"  || op == ">"  ||
        op == "<=" || op == ">=")
    {
        return std::make_unique<Type>("mute");
    }

    // Logical operators
    if (op == "chord" || op == "divis")
    {
        return std::make_unique<Type>("mute");
    }

    // the // op
    if (op == "//")
    {
        return std::make_unique<Type>("freq");
    }

    // Arithmetic operators
    if (op == "+")
    {
        if (leftType->getType() == "bar" || rightType->getType() == "bar")
            return std::make_unique<Type>("bar");
        if (*leftType == "freq" || *rightType == "freq")
            return std::make_unique<Type>("freq");
        if (*leftType == "note" || *rightType == "note")
            return std::make_unique<Type>("note");
    }

    if (op == "-" ||
        op == "*"  || op == "/" ||
        op == "%")
    {
        // If both types are identical → return that type
        if (*leftType == *rightType)
            return leftType;

        // Otherwise: promote
        if (*leftType == "freq" || *rightType == "freq")
            return std::make_unique<Type>("freq");
        if (*leftType == "note" || *rightType == "note")
            return std::make_unique<Type>("note");

        // Otherwise int
        return std::make_unique<Type>("degree");
    }

    return std::make_unique<Type>("fermata");
}

void BinaryOpExpr::analyze() const
{
    auto leftType = left != nullptr ? left->getType() : nullptr;
    auto rightType = right->getType();

    // Comparison operators
    if (op == "==" || op == "!=" ||
        op == "<"  || op == ">"  ||
        op == "<=" || op == ">=")
    {
        if (*leftType != *rightType)
        {
            throw IllegalOpOnType(token, leftType->toString(), rightType->toString(), op);
        }
    }

    // Logical operators
    if (op == "chord" || op == "divis")
    {
        if (*leftType != *rightType)
        {
            throw IllegalOpOnType(token, leftType->toString(), rightType->toString(), op);
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
    if (op == "+")
    {
        if (!(leftType->isNumberable() && rightType->isNumberable()) || (leftType->isStringable() && rightType->isStringable()))
        {
            throw IllegalOpOnType(token, leftType->toString(), rightType->toString(), "+");
        }
    }

    // Arithmetic operators
    if (op == "//"  || op == "-" ||
        op == "*"  || op == "/" ||
        op == "%")
    {
        if (!(leftType->isNumberable() && rightType->isNumberable()))
        {
            throw IllegalOpOnType(token, leftType->toString(), rightType->toString(), op);
        }
    }
}

std::string BinaryOpExpr::translateToCpp() const
{
    const bool isLeftNull = left == nullptr;
    const std::string leftStr = !isLeftNull ? left->translateToCpp() : "";
    const std::string rightStr = right->translateToCpp();
    const std::string leftType = !isLeftNull ? left->getType()->getType() : "degree";
    const std::string rightType = right->getType()->getType();
    const std::string opStr = op;
    std::ostringstream oss;

    if (op == "//")
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
    else if (opStr == "+" && leftType == "bar" && rightType != "bar")
    {
        oss << leftStr << " " << opStr << " std::to_string(" << rightStr << ")";
    }
    else if (opStr == "+" && leftType != "bar" && rightType == "bar")
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

int BinaryOpExpr::getPrecedence(const std::string& op)
{
    // Dot op
    if (op == "\\")
        return 4;

    // Multiplicative
    if (op == "*" || op == "/" || op == "%" || op == "//")
        return 3;

    // Additive
    if (op == "+" || op == "-")
        return 2;

    // Relational & Boolean
    if (op == "<"  || op == ">" ||
        op == "<=" || op == ">=" ||
        op == "chord" || op == "divis")
        return 1;

    // Equality
    if (op == "==" || op == "!=")
        return 1;

    return 0;
}
