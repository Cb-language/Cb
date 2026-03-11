#include "ExprParser.h"

#include <windows.h>

#include "TypeParser.h"

#include "errorHandling/syntaxErrors/MissingParenthesis.h"
#include "errorHandling/syntaxErrors/UnexpectedToken.h"
#include "errorHandling/syntaxErrors/InvalidExpression.h"
#include "errorHandling/syntaxErrors/MissingIdentifier.h"
#include "errorHandling/syntaxErrors/MissingSemicolon.h"

#include "errorHandling/how/HowDidYouGetHere.h"
#include "AST/statements/expression/VarCallExpr.h"
#include "symbols/Type/ClassType.h"
#include "symbols/Type/PrimitiveType.h"

#include "token/Token.h"


ExprParser::ExprParser(ParserContext& c, TypeParser& typeParser) : c(c), typeParser(typeParser)
{
}

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
        case CbTokenType::PUNCTUATION_BACKSLASH:    return "\\";
        case CbTokenType::UNARY_OP_SHARP:           return "♯";
        case CbTokenType::UNARY_OP_DOUBLE_SHARP:    return "𝄪";
        case CbTokenType::UNARY_OP_FLAT:            return "♭";
        case CbTokenType::UNARY_OP_DOUBLE_FLAT:     return "𝄫";
        case CbTokenType::UNARY_OP_NATRUAL:         return "♮";
        case CbTokenType::UNARY_OP_NOT:             return "!";
        default:                                  return "";
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

std::unique_ptr<Expr> ExprParser::parseExpr(const bool needsSemicolon, const bool allowQualifiedPaths)
{
    auto left = parseUnaryOrPrimaryExpr(needsSemicolon, allowQualifiedPaths);
    if (!left) return nullptr;

    return parseInfixExpr(0, std::move(left), needsSemicolon, allowQualifiedPaths);
}

std::unique_ptr<Expr> ExprParser::parseUnaryOrPrimaryExpr(const bool needsSemicolon, const bool allowQualifiedPaths)
{
    std::unique_ptr<Expr> expr;

    if (c.isBinaryOp())
    {
        expr = parsePrefixOperatorExpr(allowQualifiedPaths);
    }
    else if (Token classNameToken; c.matchConsume(CbTokenType::IDENTIFIER, classNameToken))
    {
        if (c.matchConsume(CbTokenType::PUNCTUATION_BACKSLASH))
            expr = parseScopeResolutionExpr(classNameToken, needsSemicolon);

        else
            expr = parseIdentifierOrCallExpr(needsSemicolon, classNameToken);
    }

    else
    {
        expr = parsePrimaryExpr(needsSemicolon, allowQualifiedPaths);
    }

    if (!expr) return nullptr;

    while (c.isUnaryOp())
    {
        if (auto call = dynamic_cast<Call*>(expr.get()))
        {
            auto callPtr = std::unique_ptr<Call>(static_cast<Call*>(expr.release()));
            expr = parsePostfixUnaryExpr(std::move(callPtr), needsSemicolon);
        }

        else
        {
            break;
        }
    }

    return expr;
}

std::unique_ptr<Expr> ExprParser::parsePrefixOperatorExpr(const bool allowQualifiedPaths)
{
    const Token& t = c.current();
    std::string op = tokenToOp(t.type);
    c.advance();

    auto operand = parseExpr(false, allowQualifiedPaths);
    if (!operand) return nullptr;

    return std::make_unique<BinaryOpExpr>(
        t,
        c.getFuncDecl(),
        op,
        nullptr,
        std::move(operand),
        c.getClassDecl()
    );
}

std::unique_ptr<Expr> ExprParser::parsePrimaryExpr(const bool needsSemicolon, const bool allowQualifiedPaths)
{
    if (c.current().isConst())
        return parseConstValueExpr();

    if (c.matchNonConsume(CbTokenType::IDENTIFIER))
        return parseIdentifierOrCallExpr(needsSemicolon);

    if (c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN))
        return parseGroupedExpr(allowQualifiedPaths);

    c.addError(std::make_unique<UnexpectedToken>(c.current()));
    return nullptr;
}

std::unique_ptr<Expr> ExprParser::parseGroupedExpr(const bool allowQualifiedPaths)
{
    c.advance(); // consume '('

    auto expr = parseExpr(false, allowQualifiedPaths);
    if (!expr) return nullptr;

    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.current())))
        return nullptr;

    return expr;
}

std::unique_ptr<Expr> ExprParser::parseInfixExpr(
    const int minPrecedence,
    std::unique_ptr<Expr> left,
    const bool needsSemicolon,
    const bool allowQualifiedPaths)
{
    while (true)
    {
        if (!(c.isBinaryOp() || (allowQualifiedPaths && c.matchNonConsume(CbTokenType::PUNCTUATION_BACKSLASH))))
            return left;

        const Token& t = c.current();
        std::string op = tokenToOp(t.type);
        const int prec = BinaryOpExpr::getPrecedence(op);

        if (prec < minPrecedence)
            return left;

        c.advance();

        auto right = parseUnaryOrPrimaryExpr(needsSemicolon, allowQualifiedPaths);
        if (!right) return nullptr;

        // If the next operator binds tighter, recurse to grab it first.
        if (c.isBinaryOp() || (allowQualifiedPaths && c.matchNonConsume(CbTokenType::PUNCTUATION_BACKSLASH)))
        {
            std::string nextOp = tokenToOp(c.current().type);
            if (const int nextPrec = BinaryOpExpr::getPrecedence(nextOp); nextPrec > prec)
            {
                right->setNeedsSemicolon(false);
                right = parseInfixExpr(prec + 1, std::move(right), needsSemicolon, allowQualifiedPaths);
                if (!right) return nullptr;
            }
        }

        if (op == "\\")
        {
            // '\' is member access — both sides must be Call nodes.
            auto callLeft  = std::unique_ptr<Call>(dynamic_cast<Call*>(left.release()));
            auto callRight = std::unique_ptr<Call>(dynamic_cast<Call*>(right.release()));

            if (!callLeft || !callRight)
            {
                c.addError(std::make_unique<HowDidYouGetHere>(t));
                return nullptr;
            }

            callRight->setIsClassItem(true);

            left = std::make_unique<DotOpExpr>(
                t,
                c.getFuncDecl(),
                std::move(callLeft),
                std::move(callRight),
                c.getClassDecl()
            );
        }

        else
        {
            left = std::make_unique<BinaryOpExpr>(
                t,
                c.getFuncDecl(),
                op,
                std::move(left),
                std::move(right),
                c.getClassDecl()
            );
        }
    }
}

std::unique_ptr<StaticDotOpExpr> ExprParser::parseScopeResolutionExpr(const Token& classNameToken, const bool needsSemicolon)
{
    if (!c.expect(CbTokenType::PUNCTUATION_BACKSLASH, std::make_unique<UnexpectedToken>(c.current())))
        return nullptr;

    std::unique_ptr<Call> right = nullptr;

    if (c.current().type == CbTokenType::IDENTIFIER && c.peek().type == CbTokenType::PUNCTUATION_PARENTHESIS_OPEN)
        right = parseFuncCallExpr(needsSemicolon);
    else
        right = parseCallExpr();

    if (!right) return nullptr;

    return std::make_unique<StaticDotOpExpr>(
        classNameToken,
        c.getFuncDecl(),
        std::make_unique<ClassType>(FQN{classNameToken.value.value()}),
        std::move(right),
        needsSemicolon,
        c.getClassDecl()
    );
}

std::unique_ptr<ConstValueExpr> ExprParser::parseConstValueExpr() const
{
    const Token& t = c.current();
    c.advance();
    Primitive type;

    switch (t.type)
    {
        case CbTokenType::CONST_BOOL:  type = Primitive::TYPE_MUTE;   break;
        case CbTokenType::CONST_STR:   type = Primitive::TYPE_BAR;    break;
        case CbTokenType::CONST_CHAR:  type = Primitive::TYPE_NOTE;   break;
        case CbTokenType::CONST_FLOAT: type = Primitive::TYPE_FREQ;   break;
        case CbTokenType::CONST_INT:   type = Primitive::TYPE_DEGREE; break;
        default:
            c.addError(std::make_unique<InvalidExpression>(t));
            return nullptr;
    }

    return std::make_unique<ConstValueExpr>(
        t,
        c.getFuncDecl(),
        std::make_unique<PrimitiveType>(type),
        t.value.value(),
        c.getClassDecl()
    );
}

std::unique_ptr<UnaryOpExpr> ExprParser::parsePostfixUnaryExpr(std::unique_ptr<Call> call, const bool needsSemicolon) const
{
    Token opToken;
    if (!c.expect(CbTokenType::UNARY_OP_SHARP,       nullptr, opToken) &&
        !c.expect(CbTokenType::UNARY_OP_DOUBLE_SHARP, nullptr, opToken) &&
        !c.expect(CbTokenType::UNARY_OP_FLAT,         nullptr, opToken) &&
        !c.expect(CbTokenType::UNARY_OP_DOUBLE_FLAT,  nullptr, opToken) &&
        !c.expect(CbTokenType::UNARY_OP_NATRUAL,      nullptr, opToken))
    {
        c.addError(std::make_unique<UnexpectedToken>(c.current()));
        return nullptr;
    }

    return std::make_unique<UnaryOpExpr>(
        call->getToken(),
        c.getFuncDecl(),
        std::move(call),
        strToUnaryOp(tokenToOp(opToken.type)),
        needsSemicolon,
        c.getClassDecl()
    );
}

std::unique_ptr<Call> ExprParser::parseVarCallExpr() const
{
    const Token& t = c.current();
    const FQN name = c.parseFQN();
    return std::make_unique<VarCallExpr>(t, c.getFuncDecl(), Var(nullptr, name), c.getClassDecl());
}


std::unique_ptr<Call> ExprParser::parseFuncCallExpr(const bool needsSemicolon)
{
    const Token& t = c.current();
    const FQN name = c.parseFQN();
    return nullptr;
}

std::unique_ptr<Call> ExprParser::parseFuncCallExprInternal(const Token& t, const FQN& name, const bool needsSemicolon)
{
    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    std::vector<std::unique_ptr<Expr>> args;
    while (!c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        auto arg = parseExpr();
        if (!arg) return nullptr;
        args.push_back(std::move(arg));

        if (!c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            if (!c.expect(CbTokenType::PUNCTUATION_COMMA, std::make_unique<UnexpectedToken>(c.current()))) return nullptr;
        }
    }
    c.advance(); // consume ')'

    if (needsSemicolon)
    {
        if (!c.expect(CbTokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;
    }

    return std::make_unique<FuncCallExpr>(
        t,
        c.getFuncDecl(),
        name,
        std::move(args),
        needsSemicolon,
        c.getClassDecl()
    );
}

std::unique_ptr<Call> ExprParser::parseIdentifierOrCallExpr(const bool needsSemicolon, std::optional<Token> firstToken)
{
    FQN name;
    Token t;
    if (firstToken.has_value()) {
        t = firstToken.value();
        name.push_back(t.value.value());
        if (c.matchConsume(CbTokenType::PUNCTUATION_BACKSLASH)) {
            FQN rest = c.parseFQN();
            name.insert(name.end(), rest.begin(), rest.end());
        }
    } else {
        t = c.current();
        name = c.parseFQN();
    }

    std::unique_ptr<Call> call;
    if (c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN)) {
        call = parseFuncCallExprInternal(t, name, needsSemicolon);
    } else {
        call = std::make_unique<VarCallExpr>(t, c.getFuncDecl(), Var(nullptr, name), c.getClassDecl());
    }

    if (!call) return nullptr;

    while (c.matchConsume(CbTokenType::PUNCTUATION_OPEN_SQUARE_BRACE))
    {
        call = parseArrayAccess(std::move(call));
        if (!call) return nullptr;
    }

    return call;
}

std::unique_ptr<Call> ExprParser::parseCallExpr(std::optional<Token> t)
{
    auto call = parseVarCallExpr();
    if (!call) return nullptr;

    while (c.matchConsume(CbTokenType::PUNCTUATION_OPEN_SQUARE_BRACE))
    {
        call = parseArrayAccess(std::move(call));
        if (!call) return nullptr;
    }

    return call;
}

std::unique_ptr<Call> ExprParser::parseArrayAccess(std::unique_ptr<Call> call)
{
    auto expr = parseExpr();

    if (c.matchConsume(CbTokenType::PUNCTUATION_OPEN_SQUARE_BRACE))
    {
        return std::make_unique<ArrayIndexingExpr>(
            c.current(),
            c.getFuncDecl(),
            std::move(call),
            std::move(expr),
            c.getClassDecl()
        );
    }
    if (!c.expect(CbTokenType::PUNCTUATION_COLON, std::make_unique<UnexpectedToken>(c.current()))) return nullptr;

    auto sliceingExpr = std::make_unique<ArraySlicingExpr>(
        c.current(),
        c.getFuncDecl(),
        std::move(call),
        c.getClassDecl()
    );

    sliceingExpr->setStart(std::move(expr));
    sliceingExpr->setStop(parseExpr());

    if (!c.matchConsume(CbTokenType::PUNCTUATION_COLON))
    {
        c.addError(std::make_unique<UnexpectedToken>(c.current()));
        return nullptr;
    }

    sliceingExpr->setStop(parseExpr());

    c.expect(CbTokenType::PUNCTUATION_CLOSE_SQUARE_BRACE, std::make_unique<MissingParenthesis>(c.current()));

    return sliceingExpr;
}
