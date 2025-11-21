#include "UnaryOpExpr.h"

UnaryOpExpr::UnaryOpExpr(std::unique_ptr<VarCallExpr> var, const UnaryOp op)  : var(std::move(var)), op(op)
{
}

bool UnaryOpExpr::isLegal() const
{
    if (var->getType() == L"degree")
    {
        return true;
    }
    return false;
}

std::string UnaryOpExpr::translateToCpp() const
{
    std::string ret = Utils::printTabs() + Utils::wstrToStr(var->getName());
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

Type UnaryOpExpr::getType() const
{
    return var->getType();
}