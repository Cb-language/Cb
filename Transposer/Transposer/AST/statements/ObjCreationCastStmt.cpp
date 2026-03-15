#include "ObjCreationCastStmt.h"

#include <algorithm>

#include "errorHandling/how/HowDidYouGetHere.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"

ObjCreationCastStmt::ObjCreationCastStmt(const Token& token, const bool hasStartingValue, std::unique_ptr<CastCallExpr> startingValue, const Var& var)
: ObjectCreationStmt(token, hasStartingValue, std::move(startingValue), var)
{
}

void ObjCreationCastStmt::analyze() const
{
    if (const auto cast = dynamic_cast<CastCallExpr*>(startingValue.get()))
    {
        cast->analyze();
        const auto type = cast->getType();
        const auto varType = var.getType();

        if (type == nullptr || varType == nullptr) throw HowDidYouGetHere(token);

        if (varType != type && type != varType)
        {
            throw IllegalTypeCast(token, type->toString(), varType->toString());
        }

        VarDeclStmt::analyze();
        return;
    }

    throw HowDidYouGetHere(token);
}

std::string ObjCreationCastStmt::translateToCpp() const
{
    std::string prefix = "";
    if (var.getStatic()) prefix = "static ";
    std::string ret = getTabs() + prefix + var.getType()->translateTypeToCpp() + " " + translateFQNtoString(var.getName());

    if (hasStartingValue && startingValue != nullptr && !var.getStatic())
    {
        ret += " = " + startingValue->translateToCpp();
    }

    ret += ";";
    return ret;
}
