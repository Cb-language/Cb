#include "ArrayDeclStmt.h"

#include <sstream>

#include "errorHandling/Error.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"
#include "errorHandling/semanticErrors/IllegalVarName.h"
#include "errorHandling/classErrors/IllegalFieldName.h"
#include "other/SymbolTable.h"
#include "other/Utils.h"

void ArrayDeclStmt::analyzeSizes() const
{
    if (symTable == nullptr) return;

    const auto t = var.getType();
    if (!t) return;

    if (t->getArrLevel() != sizes.size())
    {
        symTable->addError(std::make_unique<HowDidYouGetHere>(token));
    }

    for (const auto& size : sizes)
    {
        size->setSymbolTable(symTable);
        size->setScope(scope);
        size->setClassNode(currClass);
        size->analyze();

        const auto sizeType = size->getType();
        if (!sizeType) continue; // error already reported in analyze()

        if (!sizeType->isNumberable())
        {
            symTable->addError(std::make_unique<IllegalTypeCast>(token, sizeType->toString(), "degree"));
        }
    }
}

std::string ArrayDeclStmt::createConstructor(const IType* type, const size_t dim) const
{
    std::ostringstream oss;

    if (type->getArrLevel() == 0)
    {
        if (hasStartingValue && startingValue && startingValue->getType() && startingValue->getType()->getArrLevel() == 0)
        {
            return startingValue->translateToCpp();
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

ArrayDeclStmt::ArrayDeclStmt(const Token& token, const bool hasStartingValue,
    std::unique_ptr<Expr> startingValue, const Var& var, std::vector<std::unique_ptr<Expr>> sizes) :
        VarDeclStmt(token, hasStartingValue, std::move(startingValue), var), sizes(std::move(sizes))
{
}

void ArrayDeclStmt::analyze() const
{
    if (symTable == nullptr) return;

    // Delegate basic registration and name checking to VarDeclStmt
    VarDeclStmt::analyze();

    if (!sizes.empty())
    {
        analyzeSizes();
    }
}

std::string ArrayDeclStmt::translateToCpp() const
{
    std::ostringstream oss;

    const std::string name = translateFQNtoString(var.getName());


    oss << getTabs()
        << var.getType()->translateTypeToCpp()
        << " "
        << name
        << " = "
        << createConstructor(var.getType().get(), 0)
        << ";";

    if (hasStartingValue && startingValue != nullptr)
    {
        const auto vt = var.getType();
        if (const auto st = startingValue->getType(); vt && st)
        {
            const unsigned int varArrLevel = vt->getArrLevel();
            if (const unsigned int startArrLevel = st->getArrLevel(); startArrLevel != 0 && (varArrLevel == startArrLevel || varArrLevel == startArrLevel + 1))
            {
                oss << std::endl << getTabs() << name << " = " << startingValue->translateToCpp() << ";";
            }
        }
    }

    return oss.str();
}
