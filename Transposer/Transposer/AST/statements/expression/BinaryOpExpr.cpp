#include "BinaryOpExpr.h"

#include <sstream>

#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "other/SymbolTable.h"

BinaryOpExpr::BinaryOpExpr(const Token& token, const std::string& op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
        : Expr(token), op(op), left(std::move(left)), right(std::move(right))
{
}

std::unique_ptr<IType> BinaryOpExpr::getType() const
{
    if (left == nullptr)
    {
        return right->getType()->copy();
    }

    auto leftType = left->getType()->copy();
    const auto rightType = right->getType()->copy();

    // Comparison operators
    if (op == "==" || op == "!=" ||
        op == "<"  || op == ">"  ||
        op == "<=" || op == ">=")
    {
        return std::make_unique<PrimitiveType>(Primitive::TYPE_MUTE);
    }

    // Logical operators
    if (op == "div." || op == "non div.")
    {
        return std::make_unique<PrimitiveType>(Primitive::TYPE_MUTE);
    }

    // the // op
    if (op == "//")
    {
        return std::make_unique<PrimitiveType>(Primitive::TYPE_FREQ);
    }

    // Arithmetic operators
    if (op == "+")
    {
        if (leftType->toString() == "bar" || rightType->toString() == "bar")
            return std::make_unique<PrimitiveType>(Primitive::TYPE_BAR);
        if (leftType->toString() == "freq" || rightType->toString() == "freq")
            return std::make_unique<PrimitiveType>(Primitive::TYPE_FREQ);
        if (leftType->toString() == "note" || rightType->toString() == "note")
            return std::make_unique<PrimitiveType>(Primitive::TYPE_NOTE);
        if (leftType->toString() == "degree" || rightType->toString() == "degree")
            return std::make_unique<PrimitiveType>(Primitive::TYPE_DEGREE);
    }

    if (op == "-" ||
        op == "*"  || op == "/" ||
        op == "%")
    {
        // If both types are identical → return that type
        if (*leftType == *rightType)
            return leftType;

        // Otherwise: promote
        if (leftType->toString() == "freq" || rightType->toString() == "freq")
            return std::make_unique<PrimitiveType>(Primitive::TYPE_FREQ);
        if (leftType->toString() == "note" || rightType->toString() == "note")
            return std::make_unique<PrimitiveType>(Primitive::TYPE_NOTE);

        // Otherwise int
        return std::make_unique<PrimitiveType>(Primitive::TYPE_DEGREE);
    }

    return std::make_unique<PrimitiveType>(Primitive::TYPE_FERMATA);
}

void BinaryOpExpr::analyze() const
{
    if (symTable == nullptr) return;

    if (left != nullptr)
    {
        left->setSymbolTable(symTable);
        left->setScope(scope);
        left->setClassNode(currClass);
        left->analyze();
    }

    right->setSymbolTable(symTable);
    right->setScope(scope);
    right->setClassNode(currClass);
    right->analyze();

    const auto leftType = left != nullptr ? left->getType() : nullptr;
    const auto rightType = right->getType();

    // Comparison operators
    if (op == "==" || op == "!=" ||
        op == "<"  || op == ">"  ||
        op == "<=" || op == ">=")
    {
        if (leftType == nullptr || *leftType != *rightType)
        {
            throw IllegalOpOnType(token, leftType ? leftType->toString() : "null", rightType->toString(), op);
        }
    }

    // Logical operators
    if (op == "chord" || op == "divis")
    {
        if (leftType == nullptr || *leftType != *rightType)
        {
            throw IllegalOpOnType(token, leftType ? leftType->toString() : "null", rightType->toString(), op);
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
        if (leftType == nullptr)
        {
             if (!rightType->isNumberable()) throw IllegalOpOnType(token, "null", rightType->toString(), "+");
        }
        else if (leftType->toString() == "bar" || rightType->toString() == "bar")
        {
            // string concatenation or string + something is usually allowed in our translateToCpp
            // but we should check if at least one is stringable or both are numberable
            if (!((leftType->isStringable() || leftType->isNumberable()) && (rightType->isStringable() || rightType->isNumberable())))
            {
                throw IllegalOpOnType(token, leftType->toString(), rightType->toString(), "+");
            }
        }
        else if (!(leftType->isNumberable() && rightType->isNumberable()))
        {
            throw IllegalOpOnType(token, leftType->toString(), rightType->toString(), "+");
        }
    }

    // Arithmetic operators
    if (op == "//"  || op == "-" ||
        op == "*"  || op == "/" ||
        op == "%")
    {
        if (leftType != nullptr)
        {
            if (!(leftType->isNumberable() && rightType->isNumberable()))
            {
                throw IllegalOpOnType(token, leftType->toString(), rightType->toString(), op);
            }
        }
        else
        {
            if (!rightType->isNumberable())
            {
                throw IllegalOpOnType(token, "null", rightType->toString(), op);
            }
        }
    }
}

std::string BinaryOpExpr::translateToCpp() const
{
    const bool isLeftNull = left == nullptr;
    const std::string leftStr = !isLeftNull ? left->translateToCpp() : "";
    const std::string rightStr = right->translateToCpp();
    const std::string leftType = !isLeftNull ? left->getType()->toString() : "degree";
    const std::string rightType = right->getType()->toString();
    const std::string opStr = op;
    std::ostringstream oss;

    if (needsSemicolon)
    {
        oss << getTabs();
    }

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

    if (needsSemicolon)
    {
        oss << ";";
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
