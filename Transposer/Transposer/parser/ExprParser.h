#pragma once
#include "ParserContext.h"
#include <memory>
#include <vector>
#include <string>

#include "AST/statements/ObjCreationStmt.h"
#include "AST/statements/expression/CastCallExpr.h"
#include "AST/statements/expression/IsExpr.h"
#include "AST/statements/expression/LenExpr.h"
#include "AST/statements/expression/UnaryOpExpr.h"

class Expr;
class ConstValueExpr;
class VarReference;
class TypeParser;

using FQN = std::vector<std::string>;

class ExprParser
{
private:
    ParserContext& c;
    TypeParser& typeParser;

    std::unique_ptr<Expr> parseExprPrec(int minPrec);
    static int getOpPrecedence(const CbTokenType type);
    static bool isRightAssociative(const CbTokenType type);
    static std::string tokenToOp(const CbTokenType type);
    static UnaryOp strToUnaryOp(const std::string& str);
public:
    explicit ExprParser(ParserContext& c, TypeParser& typeParser);

    std::unique_ptr<Expr> parseExpr();
    std::unique_ptr<ConstValueExpr> parseConstValue() const;
    FQN parseFQN() const;
    std::unique_ptr<VarReference> parseFuncCall(const FQN& name);
    std::unique_ptr<Expr> parseUnaryOp();
    std::unique_ptr<Expr> parseBinaryOp(std::unique_ptr<VarReference> left);
    std::unique_ptr<VarReference> parseVarExpr();
    std::unique_ptr<VarReference> parseArrayAccess(std::unique_ptr<VarReference> ref);
    std::unique_ptr<CastCallExpr> parseCastExpr();
    std::unique_ptr<IsExpr> parseIsExpr(std::unique_ptr<VarReference> ref) const;
    std::unique_ptr<LenExpr> parseLenExpr();
};
