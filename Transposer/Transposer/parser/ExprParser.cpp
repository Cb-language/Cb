#include "ExprParser.h"
#include "TypeParser.h"
#include "AST/statements/expression/BinaryOpExpr.h"
#include "AST/statements/expression/UnaryOpExpr.h"
#include "AST/statements/expression/ConstValueExpr.h"
#include "AST/statements/expression/FuncCallExpr.h"
#include "AST/statements/expression/VarCallExpr.h"
#include "AST/statements/expression/ArrayIndexingExpr.h"
#include "AST/statements/AssignmentStmt.h"
#include "AST/statements/expression/ArraySlicingExpr.h"
#include "errorHandling/syntaxErrors/UnexpectedToken.h"
#include "errorHandling/syntaxErrors/MissingParenthesis.h"
#include "errorHandling/syntaxErrors/InvalidExpression.h"
#include "errorHandling/syntaxErrors/MissingSemicolon.h"
#include "symbols/Type/PrimitiveType.h"

ExprParser::ExprParser(ParserContext& c, TypeParser& typeParser) : c(c), typeParser(typeParser) {}

static std::string tokenToOp(const CbTokenType type)
{
    switch (type)
    {
        case CbTokenType::BINARY_OP_PLUS:           return "+";
        case CbTokenType::BINARY_OP_MINUS:          return "-";
        case CbTokenType::BINARY_OP_MULTIPLY:       return "*";
        case CbTokenType::BINARY_OP_DIVIDE:         return "/";
        case CbTokenType::BINARY_OP_MODULO:         return "%";
        case CbTokenType::BINARY_OP_EQUIAL:         return "==";
        case CbTokenType::BINARY_OP_NOT_EQUAL:      return "!=";
        case CbTokenType::BINARY_OP_LESS_THAN:      return "<";
        case CbTokenType::BINARY_OP_MORE_THAN:      return ">";
        case CbTokenType::BINARY_OP_LESSER_EQUAL:   return "<=";
        case CbTokenType::BINARY_OP_GREATER_EQUAL:  return ">=";
        case CbTokenType::BINARY_OP_AND:            return "chord";
        case CbTokenType::BINARY_OP_OR:             return "divis";
        case CbTokenType::UNARY_OP_NOT:             return "!";
        case CbTokenType::UNARY_OP_SHARP:           return "♯";
        case CbTokenType::UNARY_OP_DOUBLE_SHARP:    return "𝄪";
        case CbTokenType::UNARY_OP_FLAT:            return "♭";
        case CbTokenType::UNARY_OP_DOUBLE_FLAT:     return "𝄫";
        case CbTokenType::UNARY_OP_NATRUAL:         return "♮";
        case CbTokenType::ASSIGNMENT_OP_EQUAL:      return "=";
        case CbTokenType::ASSIGNMENT_OP_PLUS_EQUAL: return "+=";
        case CbTokenType::ASSIGNMENT_OP_MINUS_EQUAL:return "-=";
        case CbTokenType::ASSIGNMENT_OP_DIVIDE_EQUAL:return "/=";
        case CbTokenType::ASSIGNMENT_OP_MULTIPLY_EQUAL:return "*=";
        case CbTokenType::ASSIGNMENT_OP_MODULUS_EQUAL:return "%=";
        default:                                    return "";
    }
}

static UnaryOp strToUnaryOp(const std::string& str)
{
    if (str == "♯") return UnaryOp::PlusPlus;
    if (str == "𝄪") return UnaryOp::PlusPlusPlusPlus;
    if (str == "♭") return UnaryOp::MinusMinus;
    if (str == "𝄫") return UnaryOp::MinusMinusMinusMinus;
    if (str == "♮") return UnaryOp::Zero;
    if (str == "!") return UnaryOp::Not;
    return UnaryOp::Zero;
}

std::unique_ptr<Expr> ExprParser::parseExpr(const bool needsSemicolon)
{
    std::unique_ptr<Expr> left;

    if (c.matchNonConsume(CbTokenType::UNARY_OP_NOT)) // the only unary prefix to the identifier
    {
        left = parseUnaryOp();
    }

    else if (c.current().isConst())
    {
        left = parseConstValue();
    }

    else
    {
        std::unique_ptr<Call> call = parseCallExpr();

        if (c.isUnaryOp())
        {
            Token t = c.copyCurrent();
            const std::string opStr = tokenToOp(t.type);
            c.advance();
            left = std::make_unique<UnaryOpExpr>(t, c.getFuncDecl(), std::move(call), strToUnaryOp(opStr), false, c.getClassDecl());
        }

        else if (c.isAssignmentOp())
        {
            left = parseAssignmentOp(std::move(call));
        }
        else
        {
            left = std::move(call);
        }
    }

    while (c.isBinaryOp())
    {
        Token opToken = c.copyCurrent();
        std::string op = tokenToOp(opToken.type);
        c.advance();
        auto right = parseExpr();
        left = std::make_unique<BinaryOpExpr>(opToken, c.getFuncDecl(), op, std::move(left), std::move(right), c.getClassDecl());
    }

    if (needsSemicolon)
    {
        c.expect(CbTokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.copyCurrent()));
    }

    return left;
}

std::unique_ptr<ConstValueExpr> ExprParser::parseConstValue() const
{
    Token t = c.copyCurrent();
    c.advance();
    Primitive type;
    switch (t.type) {
        case CbTokenType::CONST_BOOL:  type = Primitive::TYPE_MUTE;   break;
        case CbTokenType::CONST_STR:   type = Primitive::TYPE_BAR;    break;
        case CbTokenType::CONST_CHAR:  type = Primitive::TYPE_NOTE;   break;
        case CbTokenType::CONST_FLOAT: type = Primitive::TYPE_FREQ;   break;
        case CbTokenType::CONST_INT:   type = Primitive::TYPE_DEGREE; break;
        default: return nullptr;
    }
    return std::make_unique<ConstValueExpr>(t, c.getFuncDecl(), std::make_unique<PrimitiveType>(type), t.value.value(), c.getClassDecl());
}

FQN ExprParser::parseFQN() const
{
    return c.parseFQN();
}

std::unique_ptr<Call> ExprParser::parseFuncCall(const FQN& name)
{
    Token t = c.copyCurrent();
    c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN);
    std::vector<std::unique_ptr<Expr>> args;
    while (!c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        args.push_back(parseExpr());
        if (!c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            c.expect(CbTokenType::PUNCTUATION_COMMA);
        }
    }
    return std::make_unique<FuncCallExpr>(t, c.getFuncDecl(), name, std::move(args), false, c.getClassDecl());
}

std::unique_ptr<Expr> ExprParser::parseUnaryOp()
{
    Token t = c.copyCurrent();
    const std::string opStr = tokenToOp(t.type);
    c.advance();
    
    if (opStr == "!")
    {
        auto operand = parseExpr();
        return std::make_unique<UnaryOpExpr>(t, c.getFuncDecl(), std::move(operand), UnaryOp::Not, false, c.getClassDecl());
    }
    
    return nullptr;
}

std::unique_ptr<Expr> ExprParser::parseAssignmentOp(std::unique_ptr<Call> left)
{
    Token opToken = c.copyCurrent();
    std::string op = tokenToOp(opToken.type);
    c.advance();
    auto right = parseExpr();
    return std::make_unique<AssignmentStmt>(opToken, c.getFuncDecl(), std::move(left), op, std::move(right), c.getClassDecl());
}

std::unique_ptr<Call> ExprParser::parseCallExpr()
{
    Token startToken = c.copyCurrent();
    const FQN name = parseFQN();
    std::unique_ptr<Call> call;
    if (c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN))
    {
        call = parseFuncCall(name);
    }
    else
    {
        call = std::make_unique<VarCallExpr>(startToken, c.getFuncDecl(), Var(nullptr, name), c.getClassDecl());
    }

    while (c.matchConsume(CbTokenType::PUNCTUATION_OPEN_SQUARE_BRACE))
    {
        auto index = parseExpr();
        c.expect(CbTokenType::PUNCTUATION_CLOSE_SQUARE_BRACE);
        call = std::make_unique<ArrayIndexingExpr>(c.copyCurrent(), c.getFuncDecl(), std::move(call), std::move(index), c.getClassDecl());
    }

    return call;
}

std::unique_ptr<Call> ExprParser::parseArrayAccess(std::unique_ptr<Call> call)
{
    auto expr = parseExpr();

    if (c.matchConsume(CbTokenType::PUNCTUATION_OPEN_SQUARE_BRACE))
    {
        return std::make_unique<ArrayIndexingExpr>(
            c.copyCurrent(),
            c.getFuncDecl(),
            std::move(call),
            std::move(expr),
            c.getClassDecl()
        );
    }
    if (!c.expect(CbTokenType::PUNCTUATION_COLON, std::make_unique<UnexpectedToken>(c.current()))) return nullptr;

    auto sliceingExpr = std::make_unique<ArraySlicingExpr>(
        c.copyCurrent(),
        c.getFuncDecl(),
        std::move(call),
        c.getClassDecl()
    );

    sliceingExpr->setStart(std::move(expr));
    sliceingExpr->setStop(parseExpr());

    if (!c.matchConsume(CbTokenType::PUNCTUATION_COLON))
    {
        c.addError(std::make_unique<UnexpectedToken>(c.copyCurrent()));
        return nullptr;
    }

    sliceingExpr->setStop(parseExpr());

    c.expect(CbTokenType::PUNCTUATION_CLOSE_SQUARE_BRACE, std::make_unique<MissingParenthesis>(c.current()));

    return sliceingExpr;
}
