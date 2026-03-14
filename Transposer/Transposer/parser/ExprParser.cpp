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
#include "errorHandling/syntaxErrors/InvalidExpression.h"
#include "errorHandling/syntaxErrors/UnexpectedToken.h"
#include "errorHandling/syntaxErrors/MissingParenthesis.h"
#include "symbols/Type/PrimitiveType.h"

std::unique_ptr<Expr> ExprParser::parseExprPrec(const int minPrec)
{
    std::unique_ptr<Expr> left;

    if (c.matchNonConsume(CbTokenType::UNARY_OP_NOT))
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
            left = std::make_unique<UnaryOpExpr>(t, std::move(call), strToUnaryOp(opStr), false);
        }
        else
        {
            left = std::move(call);
        }
    }

    while (c.isBinaryOp())
    {
        const CbTokenType opType = c.current().type;
        const int prec = getOpPrecedence(opType);

        // Stop if this operator binds less tightly than our caller requires
        if (prec < minPrec)
            break;

        Token opToken = c.copyCurrent();
        std::string op = tokenToOp(opToken.type);
        c.advance();

        const int nextMinPrec = isRightAssociative(opType) ? prec : prec + 1;
        auto right = parseExprPrec(nextMinPrec);

        // Build the correct AST node depending on operator kind
        switch (opType)
        {
            case CbTokenType::BINARY_OP_EQUAL:
            case CbTokenType::BINARY_OP_PLUS_EQUAL:
            case CbTokenType::BINARY_OP_MINUS_EQUAL:
            case CbTokenType::BINARY_OP_DIVIDE_EQUAL:
            case CbTokenType::BINARY_OP_MULTIPLY_EQUAL:
            case CbTokenType::BINARY_OP_MODULUS_EQUAL:
            {
                auto callLeft = std::unique_ptr<Call>(
                    dynamic_cast<Call*>(left.release())
                );
                left = std::make_unique<AssignmentStmt>(
                    opToken, std::move(callLeft), op, std::move(right)
                );
                break;
            }
            default:
            {
                left = std::make_unique<BinaryOpExpr>(
                    opToken, op, std::move(left), std::move(right)
                );
                break;
            }
        }
    }

    return left;
}

int ExprParser::getOpPrecedence(const CbTokenType type)
{
    switch (type)
    {
        // Assignment operators — lowest precedence, RIGHT-associative
    case CbTokenType::BINARY_OP_EQUAL:
    case CbTokenType::BINARY_OP_PLUS_EQUAL:
    case CbTokenType::BINARY_OP_MINUS_EQUAL:
    case CbTokenType::BINARY_OP_DIVIDE_EQUAL:
    case CbTokenType::BINARY_OP_MULTIPLY_EQUAL:
    case CbTokenType::BINARY_OP_MODULUS_EQUAL:
        return 1;

        // Logical OR
    case CbTokenType::BINARY_OP_OR:
        return 2;

        // Logical AND
    case CbTokenType::BINARY_OP_AND:
        return 3;

        // Equality
    case CbTokenType::BINARY_OP_EQUALITY:
    case CbTokenType::BINARY_OP_NOT_EQUAL:
        return 4;

        // Relational
    case CbTokenType::BINARY_OP_LESS_THAN:
    case CbTokenType::BINARY_OP_MORE_THAN:
    case CbTokenType::BINARY_OP_LESSER_EQUAL:
    case CbTokenType::BINARY_OP_GREATER_EQUAL:
        return 5;

        // Additive
    case CbTokenType::BINARY_OP_PLUS:
    case CbTokenType::BINARY_OP_MINUS:
        return 6;

        // Multiplicative — highest precedence among binary ops
    case CbTokenType::BINARY_OP_MULTIPLY:
    case CbTokenType::BINARY_OP_DIVIDE:
    case CbTokenType::BINARY_OP_MODULO:
        return 7;

    default:
        return 0;
    }
}

bool ExprParser::isRightAssociative(const CbTokenType type)
{
    switch (type)
    {
    case CbTokenType::BINARY_OP_EQUAL:
    case CbTokenType::BINARY_OP_PLUS_EQUAL:
    case CbTokenType::BINARY_OP_MINUS_EQUAL:
    case CbTokenType::BINARY_OP_DIVIDE_EQUAL:
    case CbTokenType::BINARY_OP_MULTIPLY_EQUAL:
    case CbTokenType::BINARY_OP_MODULUS_EQUAL:
        return true;
    default:
        return false;
    }
}

ExprParser::ExprParser(ParserContext& c, TypeParser& typeParser) : c(c), typeParser(typeParser) {}

std::string ExprParser::tokenToOp(const CbTokenType type)
{
    switch (type)
    {
        case CbTokenType::BINARY_OP_PLUS:           return "+";
        case CbTokenType::BINARY_OP_MINUS:          return "-";
        case CbTokenType::BINARY_OP_MULTIPLY:       return "*";
        case CbTokenType::BINARY_OP_DIVIDE:         return "/";
        case CbTokenType::BINARY_OP_MODULO:         return "%";
        case CbTokenType::BINARY_OP_EQUAL:          return "=";
        case CbTokenType::BINARY_OP_EQUALITY:       return "==";
        case CbTokenType::BINARY_OP_NOT_EQUAL:      return "!=";
        case CbTokenType::BINARY_OP_LESS_THAN:      return "<";
        case CbTokenType::BINARY_OP_MORE_THAN:      return ">";
        case CbTokenType::BINARY_OP_LESSER_EQUAL:   return "<=";
        case CbTokenType::BINARY_OP_GREATER_EQUAL:  return ">=";
        case CbTokenType::BINARY_OP_PLUS_EQUAL:     return "+=";
        case CbTokenType::BINARY_OP_MINUS_EQUAL:    return "-=";
        case CbTokenType::BINARY_OP_DIVIDE_EQUAL:   return "/=";
        case CbTokenType::BINARY_OP_MULTIPLY_EQUAL: return "*=";
        case CbTokenType::BINARY_OP_MODULUS_EQUAL:  return "%=";
        case CbTokenType::BINARY_OP_AND:            return "non div.";
        case CbTokenType::BINARY_OP_OR:             return "div.";
        case CbTokenType::UNARY_OP_NOT:             return "!";
        case CbTokenType::UNARY_OP_SHARP:           return "♯";
        case CbTokenType::UNARY_OP_DOUBLE_SHARP:    return "𝄪";
        case CbTokenType::UNARY_OP_FLAT:            return "♭";
        case CbTokenType::UNARY_OP_DOUBLE_FLAT:     return "𝄫";
        case CbTokenType::UNARY_OP_NATRUAL:         return "♮";
        default:                                    return "";
    }
}

UnaryOp ExprParser::strToUnaryOp(const std::string& str)
{
    if (str == "♯") return UnaryOp::PlusPlus;
    if (str == "𝄪") return UnaryOp::PlusPlusPlusPlus;
    if (str == "♭") return UnaryOp::MinusMinus;
    if (str == "𝄫") return UnaryOp::MinusMinusMinusMinus;
    if (str == "♮") return UnaryOp::Zero;
    if (str == "!") return UnaryOp::Not;
    return UnaryOp::Zero;
}

std::unique_ptr<Expr> ExprParser::parseExpr()
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
    else if (c.matchNonConsume(CbTokenType::KEYWORD_TRANSCRIBE))
    {
        left = parseCastExpr();
    }
    else
    {
        std::unique_ptr<Call> call = parseCallExpr();

        if (c.isUnaryOp())
        {
            Token t = c.copyCurrent();
            const std::string opStr = tokenToOp(t.type);
            c.advance();
            left = std::make_unique<UnaryOpExpr>(t, std::move(call), strToUnaryOp(opStr), false);
        }
        else if (c.isBinaryOp())
        {
            left = parseBinaryOp(std::move(call));
        }
        else if (c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN))
        {
            left = parseExpr();
            c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.copyCurrent()));
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
        left = std::make_unique<BinaryOpExpr>(opToken, op, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<ConstValueExpr> ExprParser::parseConstValue() const
{
    Token t = c.advance();

    Primitive type;
    switch (t.type)
    {
        case CbTokenType::CONST_BOOL:  type = Primitive::TYPE_MUTE;   break;
        case CbTokenType::CONST_STR:   type = Primitive::TYPE_BAR;    break;
        case CbTokenType::CONST_CHAR:  type = Primitive::TYPE_NOTE;   break;
        case CbTokenType::CONST_FLOAT: type = Primitive::TYPE_FREQ;   break;
        case CbTokenType::CONST_INT:   type = Primitive::TYPE_DEGREE; break;
        default: return nullptr;
    }
    return std::make_unique<ConstValueExpr>(t, std::make_unique<PrimitiveType>(type), t.value.value());
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
    return std::make_unique<FuncCallExpr>(t, name, std::move(args), false);
}

std::unique_ptr<Expr> ExprParser::parseUnaryOp()
{
    Token t = c.copyCurrent();
    const std::string opStr = tokenToOp(t.type);
    c.advance();
    
    if (opStr == "!")
    {
        auto operand = parseExpr();
        return std::make_unique<UnaryOpExpr>(t, std::move(operand), UnaryOp::Not, false);
    }
    
    return nullptr;
}

std::unique_ptr<Expr> ExprParser::parseBinaryOp(std::unique_ptr<Call> left)
{
    Token opToken = c.advance();
    std::string op = tokenToOp(opToken.type);
    auto right = parseExpr();

    if (op == "=" || op == "+=" || op == "-=" || op == "*=" || op == "/=" || op == "%=")
        return std::make_unique<AssignmentStmt>(opToken, std::move(left), op, std::move(right));

    return std::make_unique<BinaryOpExpr>(opToken, op, std::move(left),  std::move(right));
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
        call = std::make_unique<VarCallExpr>(startToken, Var(nullptr, name));
    }

    return parseArrayAccess(std::move(call));
}

std::unique_ptr<Call> ExprParser::parseArrayAccess(std::unique_ptr<Call> call)
{
    while (c.matchConsume(CbTokenType::PUNCTUATION_OPEN_SQUARE_BRACE))
    {
        auto firstExpr = parseExpr();
        if (!firstExpr)
        {
            c.addError(std::make_unique<InvalidExpression>(c.copyCurrent()));
            return nullptr;
        }

        if (c.matchConsume(CbTokenType::PUNCTUATION_COLON))
        {
            Token t = c.copyCurrent();

            auto slicingExpr = std::make_unique<ArraySlicingExpr>(t, std::move(call));
            slicingExpr->setStart(std::move(firstExpr));

            auto stopExpr = parseExpr();
            if (!stopExpr)
            {
                c.addError(std::make_unique<InvalidExpression>(c.copyCurrent()));
                return nullptr;
            }
            slicingExpr->setStop(std::move(stopExpr));

            if (!c.expect(CbTokenType::PUNCTUATION_COLON,
                    std::make_unique<UnexpectedToken>(c.copyCurrent())))
                return nullptr;

            auto stepExpr = parseExpr();
            if (!stepExpr)
            {
                c.addError(std::make_unique<InvalidExpression>(c.copyCurrent()));
                return nullptr;
            }
            slicingExpr->setStep(std::move(stepExpr));

            if (!c.expect(CbTokenType::PUNCTUATION_CLOSE_SQUARE_BRACE,
                    std::make_unique<MissingParenthesis>(c.copyCurrent())))
                return nullptr;

            call = std::move(slicingExpr);
        }
        else
        {
            if (!c.expect(CbTokenType::PUNCTUATION_CLOSE_SQUARE_BRACE,
                    std::make_unique<MissingParenthesis>(c.copyCurrent())))
                return nullptr;

            call = std::make_unique<ArrayIndexingExpr>(
                c.copyCurrent(), std::move(call), std::move(firstExpr));
        }
    }

    return call;
}

std::unique_ptr<CastCallExpr> ExprParser::parseCastExpr()
{
    Token startToken = c.copyCurrent();
    c.expect(CbTokenType::KEYWORD_TRANSCRIBE);
    c.expect(CbTokenType::PUNCTUATION_OPEN_TRIANGLE_BRACE);

    auto type = typeParser.parseIType();

    if (type == nullptr) return nullptr;

    c.expect(CbTokenType::PUNCTUATION_CLOSE_TRIANGLE_BRACE);

    c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.copyCurrent()));

    auto expr = parseExpr();

    c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.copyCurrent()));
    return std::make_unique<CastCallExpr>(startToken, std::move(expr), std::move(type));
}
