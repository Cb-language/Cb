#include "UnaryOpExpr.h"

UnaryOpExpr::UnaryOpExpr(Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<VarCallExpr> var, const UnaryOp op, const bool isStmt)
: Expr(scope, funcDecl), var(std::move(var)), op(op), isStmt(isStmt)
{
    if (!isStmt && op == UnaryOp::Zero)
    {
        hasParens = true;
    }
}

bool UnaryOpExpr::isLegal() const
{
    if (op == UnaryOp::Not)
    {
        return var->getType() == L"mute";
    }

    return var->getType().isPrimitive();
}

std::string UnaryOpExpr::translateToCpp() const
{
    std::string ret = getTabs() + Utils::wstrToStr(var->getName());
    switch (op)
    {
    case UnaryOp::Zero:
        if (var->getType().getType() == L"bar")
        {
            ret += " = \"\"";
        }
        else
        {
            ret += " = 0";
        }
        break;

    case UnaryOp::PlusPlus:
        ret += "++";
        break;

    case UnaryOp::MinusMinus:
        ret += "--";
        break;

    case UnaryOp::Not:
        ret += "!";
        break;
    }

    if (isStmt)
    {
        ret += ";";
    }

    return ret;
}

Type UnaryOpExpr::getType() const
{
    return var->getType();
}