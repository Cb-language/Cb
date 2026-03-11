#pragma once
#include "ParserContext.h"
#include <memory>
#include <vector>
#include <string>

class Expr;
class ConstValueExpr;
class Call;
class TypeParser;

using FQN = std::vector<std::string>;

class ExprParser
{
private:
    ParserContext& c;
    TypeParser& typeParser;

public:
    explicit ExprParser(ParserContext& c, TypeParser& typeParser);

    std::unique_ptr<Expr> parseExpr(bool needsSemicolon = false);
    std::unique_ptr<ConstValueExpr> parseConstValue() const;
    FQN parseFQN() const;
    std::unique_ptr<Call> parseFuncCall(const FQN& name);
    std::unique_ptr<Expr> parseUnaryOp();
    std::unique_ptr<Expr> parseAssignmentOp(std::unique_ptr<Call> left);
    std::unique_ptr<Call> parseCallExpr();
    std::unique_ptr<Call> parseArrayAccess(std::unique_ptr<Call> call);
};
