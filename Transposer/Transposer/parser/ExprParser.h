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

using FQN = std::vector<std::string>; // FQN = fully qualified name (name that is seperated with \)

class ExprParser
{
private:
    ParserContext& c;
    TypeParser& typeParser;

    std::unique_ptr<Expr> parseUnaryOrPrimaryExpr(bool needsSemicolon, bool allowQualifiedPaths);
    std::unique_ptr<Expr> parsePrefixOperatorExpr(bool allowQualifiedPaths);
    std::unique_ptr<Expr> parsePrimaryExpr(bool needsSemicolon, bool allowQualifiedPaths);
    std::unique_ptr<Expr> parseGroupedExpr(bool allowQualifiedPaths);
    std::unique_ptr<Expr> parseInfixExpr(int minPrecedence, std::unique_ptr<Expr> left, bool needsSemicolon, bool allowQualifiedPaths);
    std::unique_ptr<UnaryOpExpr> parsePostfixUnaryExpr(std::unique_ptr<Call> call, bool needsSemicolon) const;
    std::unique_ptr<StaticDotOpExpr> parseScopeResolutionExpr(const Token& classNameToken, bool needsSemicolon);

    std::unique_ptr<Call> parseArrayAccess(std::unique_ptr<Call> call);

    FQN parseFQN() const;
public:
    explicit ExprParser(ParserContext& c, TypeParser& typeParser);

    std::unique_ptr<Expr> parseExpr(bool needsSemicolon = false, bool allowQualifiedPaths = true);

    std::unique_ptr<ConstValueExpr> parseConstValueExpr() const;
    std::unique_ptr<Call> parseFuncCallExpr(bool needsSemicolon);
    std::unique_ptr<Call> parseVarCallExpr() const;
    std::unique_ptr<Call> parseCallExpr(std::optional<Token> t = std::nullopt);
};