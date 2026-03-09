#include "ExprParser.h"
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


ExprParser::ExprParser(ParserContext& c, TypeParser& typeParser) : c(c), typeParser(typeParser)
{
}

static std::string tokenToOp(const TokenType type)
{
    switch (type)
    {
        case TokenType::BINARY_OP_PLUS: return "+";
        case TokenType::BINARY_OP_MINUS: return "-";
        case TokenType::BINARY_OP_MULTIPLY: return "*";
        case TokenType::BINARY_OP_DIVIDE: return "/";
        case TokenType::BINARY_OP_MODULO: return "%";
        case TokenType::BINARY_OP_EQUIAL: return "==";
        case TokenType::BINARY_OP_NOT_EQUAL: return "!=";
        case TokenType::BINARY_OP_LESS_THAN: return "<";
        case TokenType::BINARY_OP_MORE_THAN: return ">";
        case TokenType::BINARY_OP_LESSER_EQUAL: return "<=";
        case TokenType::BINARY_OP_GREATER_EQUAL: return ">=";
        case TokenType::BINARY_OP_AND: return "chord";
        case TokenType::BINARY_OP_OR: return "divis";
        case TokenType::PUNCTUATION_BACKSLASH: return "\\";
        case TokenType::UNARY_OP_SHARP: return "♯";
        case TokenType::UNARY_OP_DOUBLE_SHARP: return "𝄪";
        case TokenType::UNARY_OP_FLAT: return "♭";
        case TokenType::UNARY_OP_DOUBLE_FLAT: return "𝄫";
        case TokenType::UNARY_OP_NATRUAL: return "♮";
    case TokenType::UNARY_OP_NOT: return "!";
        default: return "";
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

std::unique_ptr<Expr> ExprParser::parseExpr(const bool needsSemicolon, const bool allowBackslash)
{
    if (c.matchNonConsume(TokenType::IDENTIFIER))
    {
        if (c.isUnaryOp())
                return parseUnaryOpExpr(needsSemicolon);

        if (c.peek().type == TokenType::PUNCTUATION_BACKSLASH)
            return parseStaticDotOpExpr(needsSemicolon);
    }

    auto left = parsePrimary(needsSemicolon, allowBackslash);
    if (!left) return nullptr;

    auto expr = parseBinaryOpRight(0, std::move(left), needsSemicolon, allowBackslash);
    if (!expr) return nullptr;


    return expr;
}

std::unique_ptr<Expr> ExprParser::parsePrimary(const bool needsSemicolon, const bool allowBackslash)
{
    const Token& t = c.current();

    if (c.isBinaryOp())
    {
        const TokenType type = t.type;
        std::string op = tokenToOp(type);
        c.advance();

        auto right = parseExpr(false, allowBackslash);
        if (!right) return nullptr;

        return std::make_unique<BinaryOpExpr>(
            t,
            c.getFuncDecl(),
            op,
            nullptr,
            std::move(right),
            c.getClassDecl()
        );
    }

    if (t.isConst())
    {
        return parseConstValueExpr();
    }

    if (c.matchNonConsume(TokenType::IDENTIFIER))
    {
        if (c.peek().type == TokenType::PUNCTUATION_PARENTHESIS_OPEN)
        {
            return parseFuncCallExpr(needsSemicolon);
        }
        return parseCallExpr();
    }

    if (c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_OPEN))
    {
        c.advance();
        auto expr = parseExpr(false, allowBackslash);
        if (!expr) return nullptr;
        if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;
        return expr;
    }

    c.addError(std::make_unique<UnexpectedToken>(c.current()));
    return nullptr;
}

std::unique_ptr<Expr> ExprParser::parseBinaryOpRight(const int exprPrec, std::unique_ptr<Expr> left, const bool needsSemicolon, const bool allowBackslash, const ClassNode* classCall)
{
    while (true)
    {
        if (!(c.isBinaryOp() || (allowBackslash && c.matchNonConsume(TokenType::PUNCTUATION_BACKSLASH))))
            return left;

        const Token& t = c.current();
        std::string op = tokenToOp(t.type);
        const int prec = BinaryOpExpr::getPrecedence(op);

        if (prec < exprPrec)
            return left;

        c.advance();

        auto right = parsePrimary(needsSemicolon, allowBackslash);
        if (!right) return nullptr;

        if (c.isBinaryOp() || (allowBackslash && c.matchNonConsume(TokenType::PUNCTUATION_BACKSLASH)))
        {
            std::string nextOp = tokenToOp(c.current().type);

            if (const int nextPrec = BinaryOpExpr::getPrecedence(nextOp); nextPrec > prec)
            {
                right->setNeedsSemicolon(false);
                right = parseBinaryOpRight(prec + 1, std::move(right), needsSemicolon, allowBackslash);

                if (!right) return nullptr;
            }
        }

        if (op == "\\")
        {
            auto callLeft = std::unique_ptr<Call>(
                dynamic_cast<Call*>(left.release())
            );

            auto callRight = std::unique_ptr<Call>(
                dynamic_cast<Call*>(right.release())
            );

            if (callLeft == nullptr || callRight == nullptr)
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

std::unique_ptr<StaticDotOpExpr> ExprParser::parseStaticDotOpExpr(const bool needsSemicolon)
{
    const Token& t = c.current();
    c.advance(); // consume Identifier (Class Name)

    if (!c.expect(TokenType::PUNCTUATION_BACKSLASH, std::make_unique<UnexpectedToken>(c.current()))) return nullptr;

    std::unique_ptr<Call> right = nullptr;

    if (c.current().type == TokenType::IDENTIFIER && c.peek().type == TokenType::PUNCTUATION_PARENTHESIS_OPEN) 
        right = parseFuncCallExpr(needsSemicolon);
    else 
        right = parseCallExpr();

    if (!right) return nullptr;

    return std::make_unique<StaticDotOpExpr>(
        t,
        c.getFuncDecl(),
        std::make_unique<ClassType>(t.value.value()),
        std::move(right),
        needsSemicolon,
        c.getClassDecl());
}

std::unique_ptr<ConstValueExpr> ExprParser::parseConstValueExpr() const
{
    const Token& t = c.current();
    c.advance();
    Primitive type;

    switch (t.type)
    {
    case TokenType::CONST_BOOL:
        type = Primitive::TYPE_MUTE;
        break;
    case TokenType::CONST_STR:
        type = Primitive::TYPE_BAR;
        break;
    case TokenType::CONST_CHAR:
        type = Primitive::TYPE_NOTE;
        break;
    case TokenType::CONST_FLOAT:
        type = Primitive::TYPE_FREQ;
        break;
    case TokenType::CONST_INT:
        type = Primitive::TYPE_DEGREE;
        break;
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

std::unique_ptr<UnaryOpExpr> ExprParser::parseUnaryOpExpr(const bool needsSemicolon)
{
    const Token& t = c.current();

    std::unique_ptr<Call> call = parseCallExpr();
    if (!call) return nullptr;

    Token opToken;
    if (!c.expect(TokenType::UNARY_OP_SHARP, nullptr, opToken) &&
        !c.expect(TokenType::UNARY_OP_DOUBLE_SHARP, nullptr, opToken) &&
        !c.expect(TokenType::UNARY_OP_FLAT, nullptr, opToken) &&
        !c.expect(TokenType::UNARY_OP_DOUBLE_FLAT, nullptr, opToken) &&
        !c.expect(TokenType::UNARY_OP_NATRUAL, nullptr, opToken))
    {
        c.addError(std::make_unique<UnexpectedToken>(c.current()));
        return nullptr;
    }

    return std::make_unique<UnaryOpExpr>(
        t,
        c.getFuncDecl(),
        std::move(call),
        strToUnaryOp(tokenToOp(opToken.type)),
        needsSemicolon,
        c.getClassDecl()
    );
}

std::unique_ptr<Call> ExprParser::parseVarCallExpr(const bool needsSemicolon) const
{
    const Token& t = c.current();
    c.advance();
    
    return std::make_unique<VarCallExpr>(t, c.getFuncDecl(), Var(nullptr, t.value.value()), c.getClassDecl());
}

std::unique_ptr<Call> ExprParser::parseFuncCallExpr(const bool needsSemicolon)
{
    const Token& t = c.current();

    Token nameToken;
    if (!c.expect(TokenType::IDENTIFIER, std::make_unique<MissingIdentifier>(t), nameToken)) return nullptr;
        const std::string name = nameToken.value.value();

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    std::vector<std::unique_ptr<Expr>> args;
    while (!c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        auto arg = parseExpr();
        if (!arg) return nullptr;
        args.push_back(std::move(arg));

        if (!c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            if (!c.expect(TokenType::PUNCTUATION_COMMA, std::make_unique<UnexpectedToken>(c.current()))) return nullptr;
        }
    }
    c.advance(); // eat close parenthesis

    if (needsSemicolon)
    {
        if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;
    }

    auto call = std::make_unique<FuncCallExpr>(
        t,
        c.getFuncDecl(),
        name,
        std::move(args),
        needsSemicolon,
        c.getClassDecl()
    );

    return call;
}

std::unique_ptr<Call> ExprParser::parseCallExpr()
{
    auto call = parseVarCallExpr();
    if (!call) return nullptr;

    while (c.matchNonConsume(TokenType::PUNCTUATION_OPEN_SQUARE_BRACE))
    {
        call = parseArrayAccess(std::move(call));
        if (!call) return nullptr;
    }

    return call;
}

std::unique_ptr<Call> ExprParser::parseArrayAccess(std::unique_ptr<Call> call)
{
    if (c.matchNonConsume(TokenType::PUNCTUATION_OPEN_SQUARE_BRACE))
    {
        // check for slicing vs indexing
        size_t lookahead = c.getPos() + 1;
        bool isSlicing = false;
        int depth = 1;
        while (lookahead < c.getTokens().size() && depth > 0)
        {
            if (c.getTokens()[lookahead].type == TokenType::PUNCTUATION_OPEN_SQUARE_BRACE) depth++;
            else if (c.getTokens()[lookahead].type == TokenType::PUNCTUATION_CLOSE_SQUARE_BRACE) depth--;
            else if (depth == 1 && c.getTokens()[lookahead].type == TokenType::PUNCTUATION_COLON)
            {
                isSlicing = true;
                break;
            }
            lookahead++;
        }

        if (isSlicing) return parseArraySlicingExpr(std::move(call));
        else return parseArrayIndexingExpr(std::move(call));
    }
    return call;
}

std::unique_ptr<Call> ExprParser::parseArraySlicingExpr(std::unique_ptr<Call> call)
{
    const Token& t = c.current();
    c.advance();

    std::unique_ptr<Expr> start = nullptr;
    std::unique_ptr<Expr> stop = nullptr;
    std::unique_ptr<Expr> step = nullptr;

    if (!c.matchNonConsume(TokenType::PUNCTUATION_COLON))
    {
        start = parseExpr();
    }

    if (!c.expect(TokenType::PUNCTUATION_COLON, std::make_unique<UnexpectedToken>(c.current()))) return nullptr;

    if (!c.matchNonConsume(TokenType::PUNCTUATION_COLON) && !c.matchNonConsume(TokenType::PUNCTUATION_CLOSE_SQUARE_BRACE))
    {
        stop = parseExpr();
    }

    if (c.matchNonConsume(TokenType::PUNCTUATION_COLON))
    {
        c.advance();
        step = parseExpr();
    }

    if (!c.expect(TokenType::PUNCTUATION_CLOSE_SQUARE_BRACE, std::make_unique<UnexpectedToken>(c.current()))) return nullptr;

    return std::make_unique<ArraySlicingExpr>(
        t,
        c.getFuncDecl(),
        std::move(call),
        std::move(start),
        std::move(stop),
        std::move(step),
        c.getClassDecl()
    );
}

std::unique_ptr<Call> ExprParser::parseArrayIndexingExpr(std::unique_ptr<Call> call)
{
    const Token& t = c.current();
    c.advance();

    auto index = parseExpr();
    if (!index) return nullptr;

    if (!c.expect(TokenType::PUNCTUATION_CLOSE_SQUARE_BRACE, std::make_unique<UnexpectedToken>(c.current()))) return nullptr;

    return std::make_unique<ArrayIndexingExpr>(
        t,
        c.getFuncDecl(),
        std::move(call),
        std::move(index),
        c.getClassDecl()
    );
}
