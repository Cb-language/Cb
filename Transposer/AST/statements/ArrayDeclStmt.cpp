#include "ArrayDeclStmt.h"

#include <sstream>

bool ArrayDeclStmt::isLegalSizes() const
{
    if (var.getType()->getArrLevel() != sizes.size())
    {
        return false;
    }

    for (const auto& size : sizes)
    {
        if (!size->isLegal()|| !size->getType()->isNumberable())
        {
            return false;
        }
    }

    return true;
}

std::string ArrayDeclStmt::createConstructor(IType* type, const size_t dim) const
{
    std::ostringstream oss;

    if (type->getArrLevel() == 0)
    {
        if (hasStartingValue && startingValue->getType()->getArrLevel() == 0)
        {
            return startingValue->translateToCpp();
        }

        if (type->isPrimitive())
        {
            if (type->getType() == L"bar")
            {
                return "\"\"";
            }

            return "0";
        }

        return type->translateTypeToCpp() + "()";
    }

    oss << type->translateTypeToCpp()
        << "("
        << sizes.at(dim)->translateToCpp()
        << ", "
        << createConstructor(type->getArrType().get(), dim + 1)
        << ")";

    return oss.str();
}

ArrayDeclStmt::ArrayDeclStmt(const Token& token, Scope* scope, FuncDeclStmt* funcDecl, const bool hasStartingValue,
                             std::unique_ptr<Expr> startingValue, const Var& var, std::vector<std::unique_ptr<Expr>> sizes) :
        VarDeclStmt(token, scope, funcDecl, hasStartingValue, std::move(startingValue), var), sizes(std::move(sizes))
{
}

bool ArrayDeclStmt::isLegal() const
{
    if (!sizes.empty() && !isLegalSizes())
    {
        return false;
    }

    if (hasStartingValue)
    {
        const std::unique_ptr<IType> startType = startingValue->getType()->copy();
        const std::unique_ptr<IType> varType = var.getType()->copy();
        const unsigned int arrLevel = varType->getArrLevel();
        const unsigned int startArrLevel = startType->getArrLevel();

        if (arrLevel == 0)
        {
            return false;
        }

        if (arrLevel == startArrLevel) // Array<int> x = 3; Array<int> y = x;
        {
            return *varType == *startType;
        }
        else if (arrLevel == startArrLevel + 1) // Array<int> x = 3; or Array<int> y; Array<Array<int>> x = y;
        {
            return *(varType->getArrType()) == *startType;
        }
        else if (arrLevel > 1 && startArrLevel == 0) // Array<Array<int>> y = 3;
        {
            std::unique_ptr<IType> type = varType->getArrType()->copy();

            while (type->getArrLevel() > 0)
            {
                type = type->getArrType()->copy();
            }

            return *type == *startType;
        }

        return false;
    }

    return true;
}

std::string ArrayDeclStmt::translateToCpp() const
{
    std::ostringstream oss;

    const std::string name = Utils::wstrToStr(var.getName());


    oss << getTabs()
        << var.getType()->translateTypeToCpp()
        << " "
        << name
        << " = "
        << createConstructor(var.getType().get(), 0)
        << ";";

    if (hasStartingValue && startingValue != nullptr)
    {
        const unsigned int varArrLevel = var.getType()->getArrLevel();
        const unsigned int startArrLevel = startingValue->getType()->getArrLevel();
        if (startArrLevel != 0 && (varArrLevel == startArrLevel || varArrLevel == startArrLevel + 1))
        {
            oss << std::endl << getTabs() << name << " = " << startingValue->translateToCpp() << ";";
        }
    }

    return oss.str();
}