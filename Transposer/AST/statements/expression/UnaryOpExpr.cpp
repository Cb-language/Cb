#include "UnaryOpExpr.h"

UnaryOpExpr::UnaryOpExpr(Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<Call> call, const UnaryOp op, const bool isStmt)
: Expr(scope, funcDecl), call(std::move(call)), op(op), isStmt(isStmt)
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
        return *(call->getType()) == L"mute";
    }

    return call->getType()->getArrLevel() == 0 && call->getType()->isPrimitive();
}

std::string UnaryOpExpr::translateToCpp() const
{
    std::string ret = getTabs() + call->translateToCpp();
    switch (op)
    {
    case UnaryOp::Zero:
        if (call->getType()->getType() == L"bar")
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

std::unique_ptr<IType> UnaryOpExpr::getType() const
{
    return call->getType()->copy();
}