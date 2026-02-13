#include "UnaryOpExpr.h"

#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"

UnaryOpExpr::UnaryOpExpr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, ClassNode* currClass,
    std::unique_ptr<Call> call, const UnaryOp op, const bool isStmt)
        : Expr(token, scope, funcDecl, currClass), call(std::move(call)), op(op), isStmt(isStmt)
{
    if (!isStmt && op == UnaryOp::Zero)
    {
        hasParens = true;
    }
}

void UnaryOpExpr::analyze() const
{
    if (op == UnaryOp::Not && *(call->getType()) != L"mute")
    {
        throw IllegalTypeCast(token, call->getType()->toString(), "mute");
    }

    if (call->getType()->getArrLevel() != 0 || !call->getType()->isPrimitive())
    {
        throw IllegalOpOnType(token, call->getType()->toString());
    }
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