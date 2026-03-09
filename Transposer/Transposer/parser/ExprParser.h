#pragma once
#include "ParserContext.h"

#include "AST/statements/expression/BinaryOpExpr.h"
#include "AST/statements/expression/UnaryOpExpr.h"
#include "AST/statements/expression/ConstValueExpr.h"
#include "AST/statements/expression/FuncCallExpr.h"
#include "AST/statements/expression/StaticDotOpExpr.h"
#include "AST/statements/expression/DotOpExpr.h"
#include "AST/statements/expression/ArraySlicingExpr.h"
#include "AST/statements/expression/ArrayIndexingExpr.h"

class TypeParser;
class StmtParser;
class Expr;
class StaticDotOpExpr;
class ConstValueExpr;
class UnaryOpExpr;
class Call;
class ClassNode;

class ExprParser
{
private:
    ParserContext& c;
    TypeParser& typeParser;

public:
    explicit ExprParser(ParserContext& c, TypeParser& typeParser);

    std::unique_ptr<Expr> parseExpr(const bool needsSemicolon = false, const bool allowBackslash = true);
    std::unique_ptr<Expr> parsePrimary(const bool needsSemicolon = false, const bool allowBackslash = true, const ClassNode* classCall = nullptr);
    std::unique_ptr<Expr> parseBinaryOpRight(const int exprPrec, std::unique_ptr<Expr> left, const bool needsSemicolon = false, const bool allowBackslash = true, const ClassNode* classCall = nullptr);
    std::unique_ptr<StaticDotOpExpr> parseStaticDotOpExpr(const bool needsSemicolon = false);
    std::unique_ptr<ConstValueExpr> parseConstValueExpr() const;
    std::unique_ptr<UnaryOpExpr> parseUnaryOpExpr(const bool needsSemicolon = false);
    std::unique_ptr<Call> parseVarCallExpr(const bool needsSemicolon = false);
    std::unique_ptr<Call> parseFuncCallExpr(const bool needsSemicolon = false);
    std::unique_ptr<Call> parseCallExpr(const ClassNode* classCall = nullptr);
    std::unique_ptr<Call> parseArrayAccess(std::unique_ptr<Call> call);
    std::unique_ptr<Call> parseArraySlicingExpr(std::unique_ptr<Call> call);
    std::unique_ptr<Call> parseArrayIndexingExpr(std::unique_ptr<Call> call);

    void parse() const;
};
