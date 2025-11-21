#include "Parser.h"

#include <memory>
#include <sstream>
#include <vector>

#include "errorHandling/lexicalErrors/InvalidIdentifier.h"
#include "errorHandling/lexicalErrors/UnexpectedEOF.h"
#include "errorHandling/syntaxErrors/InvalidExpression.h"
#include "errorHandling/syntaxErrors/MisplacedKeyword.h"
#include "errorHandling/syntaxErrors/MissingBrace.h"
#include "errorHandling/syntaxErrors/MissingIdentifier.h"
#include "errorHandling/syntaxErrors/MissingSemicolon.h"
#include "errorHandling/syntaxErrors/UnexpectedToken.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), len(tokens.size()), pos(0), symTable(SymbolTable())
{
    Utils::init(&symTable);
}

Parser::~Parser()
{
    Utils::reset();
}

void Parser::parse()
{
    while (!isAtEnd())
    {
        if (match(Token::TYPE))
        {
            stmts.push_back(parseVarDecStmt());
        }
        else if (match(Token::IDENTIFIER) && (peek().type == Token::OP_ASSIGNMENT || (peek().type == Token::PUNCTUATION && peek().value == L"║"))) // if an assignment (have identifier [= expr] ║ )
        {
            stmts.push_back(parseAssignmentStmt());
        }
        else if (match(Token::KEYWORD, L"hear"))
        {
            stmts.push_back(parseHearStmt());
        }
        else if (match(Token::KEYWORD, L"play"))
        {
            stmts.push_back(parsePlayStmt());
        }
        else  if (match(Token::IDENTIFIER) && peek().type == Token::OP_UNARY)
        {
            stmts.push_back(parseUnaryOpExpr());
        }
        else if (match(Token::COMMENT_MULTI) || match(Token::COMMENT_SINGLE))
        {
            advance();
        }
        else
        {
            throw UnexpectedToken(current());
        }
    }
}

bool Parser::checkLegal() const
{
    for (const auto& stmt : stmts)
    {
        if (!stmt->isLegal())
        {
            return false;
        }
    }

    return true;
}

std::string Parser::translateToCpp() const
{
    std::ostringstream oss;
    oss << "#include <iostream>" << std::endl;
    oss << "#include <string>" << std::endl;
    oss << std::endl << "int main()" << std::endl << "{" << std::endl;

    for (const auto& stmt : stmts)
    {
        oss << std:: endl <<stmt->translateToCpp();
    }

    oss << std::endl << "\t" << "return 0;" << std::endl << "}" << std::endl;

    return oss.str();
}

const Token& Parser::current() const
{
    return tokens[pos];
}

const Token& Parser::advance()
{
    if (isAtEnd())
    {
        throw UnexpectedEOF(tokens.back());
    }
    return tokens[++pos];
}

const Token& Parser::peek() const
{
    if (isAtEnd())
    {
        throw UnexpectedEOF(tokens.back());
    }

    return tokens[pos + 1];
}

const Token& Parser::prev() const
{
    if (pos <= 0)
    {
        throw UnexpectedEOF(tokens.front());
    }
    return tokens[pos - 1];
}

bool Parser::isAtEnd() const
{
    return pos + 1 > len;
}

bool Parser::match(const Token::TokenType type) const
{
    return current().type == type;
}

bool Parser::match(const Token::TokenType type, const std::wstring& value) const
{
    const Token t = current();

    return t.type == type && t.value == value;
}

void Parser::expect(const Token::TokenType type)
{
    if (!match(type))
    {
        throw UnexpectedToken(current());
    }
    advance();
}

void Parser::expect(const Token::TokenType type, const std::wstring& value)
{
    if (!match(type, value))
    {
        throw UnexpectedToken(current());
    }
    advance();
}

void Parser::expect(const Token::TokenType type, const Error& err)
{
    if (!match(type))
    {
        throw err;
    }
    advance();
}

void Parser::expect(const Token::TokenType type, const std::wstring& value, const Error& err)
{
    if (!match(type, value))
    {
        throw err;
    }
    advance();
}

const Token& Parser::expectAndGet(const Token::TokenType type)
{
    expect(type);
    return prev();
}

const Token& Parser::expectAndGet(const Token::TokenType type, const std::wstring& value)
{
    expect(type, value);
    return prev();
}

const Token& Parser::expectAndGet(const Token::TokenType type, const Error& err)
{
    expect(type, err);
    return prev();
}

const Token& Parser::expectAndGet(const Token::TokenType type, const std::wstring& value, const Error& err)
{
    expect(type, value, err);
    return prev();
}

std::unique_ptr<VarDecStmt> Parser::parseVarDecStmt()
{
    const Type varType(expectAndGet(Token::TYPE).value);

    const std::wstring varName = expectAndGet(
        Token::IDENTIFIER, MissingIdentifier(current())
        ).value;

    const Var var(varType, varName);
    const Token identifierToken = prev();

    if (match(Token::PUNCTUATION, L"║"))
    {
        symTable.addVar(varType, identifierToken);
        advance();
        return std::make_unique<VarDecStmt>(false, nullptr, var);
    }

    expect(Token::OP_ASSIGNMENT, L"=");

    auto expr = parseExpr();
    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));
    symTable.addVar(varType, identifierToken); // to avoid degree x = x + 1║ ...
    return std::make_unique<VarDecStmt>(true, std::move(expr), var);
}

std::unique_ptr<AssignmentStmt> Parser::parseAssignmentStmt()
{
    auto var = parseVarCallExpr();

    const std::wstring op = expectAndGet(Token::OP_ASSIGNMENT).value;


    auto expr = parseExpr();
    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));

    return std::make_unique<AssignmentStmt>(std::move(var), op, std::move(expr));
}

std::unique_ptr<HearStmt> Parser::parseHearStmt()
{
    std::vector<std::unique_ptr<VarCallExpr>> vars;
    expect(Token::KEYWORD, L"hear");
    expect(
        Token::PUNCTUATION, L"(", MissingBrace(current())
        );

    while (true)
    {
        vars.push_back(parseVarCallExpr());

        if (match(Token::PUNCTUATION, L")"))
        {
            advance();
            break;
        }

        expect(
            Token::PUNCTUATION, L",", InvalidExpression(current())
            );
    }

    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));

    return std::make_unique<HearStmt>(std::move(vars));
}

std::unique_ptr<PlayStmt> Parser::parsePlayStmt()
{
    std::vector<std::unique_ptr<ConstValueExpr>> args;

    expect(Token::KEYWORD, L"play");
    expect(Token::PUNCTUATION, L"(", MissingBrace(current()));
    std::vector<std::unique_ptr<VarCallExpr>> vars;
    if (!match(Token::KEYWORD, L"play") && !match(Token::KEYWORD, L"playBar"))
    {
        throw UnexpectedToken(current());
    }
    const bool newline = current().value == L"playBar";
    advance();
    expect(
        Token::PUNCTUATION, L"(", MissingBrace(current())
        );

    while (true)
    {

        if (match(Token::PUNCTUATION, L")"))
        {
            advance();
            break;
        }

        if (current().type == Token::CONST_STR)
        {
            args.push_back((parseConstValueExpr()));
        }
        else if (current().type == Token::CONST_CHAR)
        {
            args.push_back(parseConstValueExpr());
        }
        else if (current().type == Token::IDENTIFIER)
        {
            auto var = parseVarCallExpr();
            args.push_back(std::make_unique<ConstValueExpr>(var->getType(), var->getName()));
        }
        else
        {
            InvalidExpression(current());
        }

        if (match(Token::PUNCTUATION, L","))
        {
            advance();
        }
    }
    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));

    return std::make_unique<PlayStmt>(args);
}


std::unique_ptr<Expr> Parser::parseExpr(const bool hasParens)
{
    if (match(Token::IDENTIFIER) && peek().type == Token::OP_UNARY)
    {
        return parseUnaryOpExpr();
    }

    auto left = parsePrimary();

    auto expr = parseBinaryOpRight(0, std::move(left));

    if (hasParens)
        expr->setHasParens(true); // only wrap the top-level expr

    return expr;
}

std::unique_ptr<Expr> Parser::parsePrimary()
{
    if (match(Token::CONST_BOOL) || match(Token::CONST_STR) ||
        match(Token::CONST_INT)  || match(Token::CONST_FLOAT) ||
        match(Token::CONST_CHAR))
    {
        return parseConstValueExpr();
    }

    if (match(Token::IDENTIFIER))
    {
        return parseVarCallExpr();
    }

    if (match(Token::PUNCTUATION, L"("))
    {
        advance();
        auto expr = parseExpr(true);
        expect(Token::PUNCTUATION, L")");
        return expr;
    }

    throw UnexpectedToken(current());
}

std::unique_ptr<Expr> Parser::parseBinaryOpRight(int exprPrec, std::unique_ptr<Expr> left)
{
    while (true)
    {
        if (!match(Token::OP_BINARY))
            return left;

        std::wstring op = current().value;
        int prec = BinaryOpExpr::getPrecedence(op);

        if (prec < exprPrec)
            return left;

        advance();

        auto right = parsePrimary();

        // Lookahead for next operator to handle higher precedence
        if (match(Token::OP_BINARY))
        {
            int nextPrec = BinaryOpExpr::getPrecedence(current().value);
            if (nextPrec > prec)
            {
                right = parseBinaryOpRight(prec + 1, std::move(right));
            }
        }

        left = std::make_unique<BinaryOpExpr>(op, std::move(left), std::move(right));
    }
}

std::unique_ptr<ConstValueExpr> Parser::parseConstValueExpr()
{
    const Token t = current();
    const Token::TokenType tokenType = t.type;
    std::wstring type;

    switch (tokenType)
    {
    case Token::CONST_BOOL:
        type = L"mute";
        break;
    case Token::CONST_STR:
        type = L"bar";
        break;
    case Token::CONST_CHAR:
        type = L"note";
        break;
    case Token::CONST_FLOAT:
        type = L"freq";
        break;
    case Token::CONST_INT:
        type = L"degree";
        break;
    default:
        throw InvalidExpression(t);
    }

    advance();
    return std::make_unique<ConstValueExpr>(Type(type), t.value);
}

std::unique_ptr<VarCallExpr> Parser::parseVarCallExpr()
{
    const std::optional<Var> var = symTable.getVar(
        expectAndGet(Token::IDENTIFIER, MissingIdentifier(current())
            ).value);

    if (!var.has_value())
    {
        throw InvalidIdentifier(prev());
    }

    return std::make_unique<VarCallExpr>(var.value());
}

std::unique_ptr<UnaryOpExpr> Parser::parseUnaryOpExpr()
{
    auto expr = parseVarCallExpr();
    UnaryOp op;

    const std::wstring value = expectAndGet(Token::OP_UNARY).value;

    if (value == L"♮")
    {
        op = UnaryOp::Zero;
    }
    else if (value == L"♯")
    {
        op = UnaryOp::PlusPlus;
    }
    else if (value == L"♭")
    {
        op = UnaryOp::MinusMinus;
    }
    else
    {
        throw UnexpectedToken(current());
    }

    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));

    return std::make_unique<UnaryOpExpr>(std::move(expr), op);
}