#include "ArrayDeclStmt.h"

#include <sstream>

ArrayDeclStmt::ArrayDeclStmt(Scope* scope, FuncDeclStmt* funcDecl, const bool hasStartingValue,
                             std::unique_ptr<Expr> startingValue, const Var& var, std::unique_ptr<Expr> size) :
        VarDeclStmt(scope, funcDecl, hasStartingValue, std::move(startingValue), var),
        size(std::move(size))
{
}

bool ArrayDeclStmt::isLegal() const
{
    if (hasStartingValue)
    {
        if (size && (!size->isLegal() || !size->getType()->isNumberable()))
        {
            return false;
        }

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
        else if (arrLevel == startArrLevel + 1) // Array<int> x = 3;
        {
            return *(varType->getArrType()) == *startType;
        }

        return false;
    }

    return true;
}

std::string ArrayDeclStmt::translateToCpp() const
{
    std::ostringstream oss;
    const std::unique_ptr<IType> startType = startingValue->getType()->copy();
    const std::unique_ptr<IType> varType = var.getType()->copy();
    const unsigned int arrLevel = varType->getArrLevel();
    const unsigned int startArrLevel = startType->getArrLevel();

    std::string sizeStr = "";

    if (size != nullptr)
    {
        sizeStr = size->translateToCpp();
    }

    oss << getTabs() << varType->translateTypeToCpp() << " " << Utils::wstrToStr(var.getName()) << "(" << sizeStr;

    if (hasStartingValue && (arrLevel == startArrLevel || arrLevel == startArrLevel + 1))
    {
        if (!sizeStr.empty())
        {
            oss << ", ";
        }

        oss << startingValue->translateToCpp();

        if (arrLevel == startArrLevel + 1 && startArrLevel > 0)
        {
            oss << ", true";
        }

        oss << ");";
    }
    else
    {
        oss << ");";
    }

    return oss.str();
}