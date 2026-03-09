#pragma once
#include "ParserContext.h"

class ExprParser
{
private:
    ParserContext& c;

    // std::unique_ptr<Expr> parseExpr(const bool hasParens = false, const bool isStmt = false, const bool allowBackslash = true);
    // std::unique_ptr<Expr> parsePrimary(const bool isStmt = false, const bool allowBackslash = true, const ClassNode* classCall = nullptr);
    // std::unique_ptr<Expr> parseBinaryOpRight(int exprPrec, std::unique_ptr<Expr> left, const bool isStmt = false, const bool allowBackslash = true, const ClassNode* classCall = nullptr);
    // std::unique_ptr<StaticDotOpExpr> parseStaticDotOpExpr(const bool isStmt = false);
    // std::unique_ptr<ConstValueExpr> parseConstValueExpr();
    // std::unique_ptr<UnaryOpExpr> parseUnaryOpExpr(const bool isStmt = false);
    // std::unique_ptr<Call> parseVarCallExpr(const bool isStmt = false);
    // std::unique_ptr<Call> parseFuncCallExpr(const bool isStmt = false);
    // std::unique_ptr<Call> parseCallExpr(const ClassNode* classCall = nullptr);
    // std::unique_ptr<Call> parseArrayAccess(std::unique_ptr<Call> call);
    // std::unique_ptr<Call> parseArraySlicingExpr(std::unique_ptr<Call> call);
    // std::unique_ptr<Call> parseArrayIndexingExpr(std::unique_ptr<Call> call);

public:
    explicit ExprParser(ParserContext& c);

    void parse() const;
};
