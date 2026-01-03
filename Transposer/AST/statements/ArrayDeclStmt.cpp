#include "ArrayDeclStmt.h"

#include <sstream>

#include "errorHandling/Error.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"

void ArrayDeclStmt::analyzeSizes() const
{
    // Can't get here but always check
    if (var.getType()->getArrLevel() != sizes.size())
    {
        throw HowDidYouGetHere(token);
    }

    for (const auto& size : sizes)
    {
        size->analyze();
        if (!size->getType()->isNumberable())
        {
            throw IllegalTypeCast(token, Utils::wstrToStr(size->getType()->getType()), "degree");
        }
    }
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

void ArrayDeclStmt::analyze() const
{
    if (!sizes.empty())
    {
        analyzeSizes();
    }

    if (hasStartingValue)
    {
        const std::unique_ptr<IType> startType = startingValue->getType()->copy();
        const std::unique_ptr<IType> varType = var.getType()->copy();
        const unsigned int arrLevel = varType->getArrLevel();
        const unsigned int startArrLevel = startType->getArrLevel();

        // Can't get here but always check
        if (arrLevel == 0)
        {
            throw HowDidYouGetHere(token);
        }

        if (arrLevel == startArrLevel) // Array<int> x = 3; Array<int> y = x;
        {
            if (*varType != *startType)
            {
                throw IllegalTypeCast(token, startType->toString(), varType->toString());
            }
        }
        else if (arrLevel == startArrLevel + 1) // Array<int> x = 3; or Array<int> y; Array<Array<int>> x = y;
        {
            if (*(varType->getArrType()) != *startType)
            {
                throw IllegalTypeCast(token, startType->toString(), varType->getArrType()->toString());
            }
        }
        else if (arrLevel > 1 && startArrLevel == 0) // Array<Array<int>> y = 3;
        {
            std::unique_ptr<IType> type = varType->getArrType()->copy();

            while (type->getArrLevel() > 0)
            {
                type = type->getArrType()->copy();
            }

            if (*type != *startType)
            {
                throw IllegalTypeCast(token, startType->toString(), type->toString());
            }
        }
    }
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