#include "UnaryOperatorExpr.h"

UnaryOperatorExpr::UnaryOperatorExpr(std::unique_ptr<VarCallExpr> var, const UnaryOp op)  : var(std::move(var)), op(op)
{
}

bool UnaryOperatorExpr::isLegal() const
{
    if (var->getType() == L"degree")
    {
        return true;
    }
    return false;
}

std::string UnaryOperatorExpr::translateToCpp() const
{
    std::string ret = Utils::wstrToStr(var->getName());
    switch (op)
    {
        case UnaryOp::Zero:
            ret += " = 0";
            break;

        case UnaryOp::PlusPlus:
            ret += "++";
            break;

        case UnaryOp::MinusMinus:
            ret += "--";
            break;
    }
    ret += ";";

    return ret;
}

Type UnaryOperatorExpr::getType() const
{
    return var->getType();
}