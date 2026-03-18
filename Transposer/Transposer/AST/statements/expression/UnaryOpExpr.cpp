#include "UnaryOpExpr.h"

#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"
#include "other/SymbolTable.h"

UnaryOpExpr::UnaryOpExpr(const Token& token,
    std::unique_ptr<Expr> operand, const UnaryOp op, const bool needsSemicolon)
        : Expr(token), operand(std::move(operand)), op(op)
{
    this->needsSemicolon = needsSemicolon;
    if (!needsSemicolon && op == UnaryOp::Zero)
    {
        hasParens = true;
    }
}

void UnaryOpExpr::analyze() const
{
    if (symTable == nullptr) return;

    operand->setSymbolTable(symTable);
    operand->setScope(scope);
    operand->setClassNode(currClass);
    operand->analyze();

    if (op == UnaryOp::Not && operand->getType()->toString() != "mute")
    {
        symTable->addError(std::make_unique<IllegalTypeCast>(token, operand->getType()->toString(), "mute"));
    }

    if (operand->getType()->getArrLevel() != 0 || !operand->getType()->isPrimitive())
    {
        symTable->addError(std::make_unique<IllegalOpOnType>(token, operand->getType()->toString()));
    }
}

std::string UnaryOpExpr::translateToCpp() const
{
    std::string ret = needsSemicolon ? getTabs() : "";
    
    if (op == UnaryOp::Not)
    {
        ret += "!" + operand->translateToCpp();
    }
    else
    {
        ret += operand->translateToCpp();
        switch (op)
        {
        case UnaryOp::Zero:
            if (operand->getType()->toString() == "bar")
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

        case UnaryOp::PlusPlusPlusPlus:
            ret += " += 2";
            break;

        case UnaryOp::MinusMinusMinusMinus:
            ret += " -= 2";
            break;
        }
    }

    if (needsSemicolon)
    {
        ret += ";";
    }

    return ret;
}

std::unique_ptr<IType> UnaryOpExpr::getType() const
{
    return operand->getType()->copy();
}

void UnaryOpExpr::setSymbolTable(SymbolTable* symTable) const
{
    Stmt::setSymbolTable(symTable);
    operand->setSymbolTable(symTable);
}
