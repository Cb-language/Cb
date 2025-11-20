#include "Parser.h"

#include <memory>
#include <vector>

#include "errorHandling/lexicalErrors/InvalidIdentifier.h"
#include "errorHandling/lexicalErrors/UnexpectedEOF.h"
#include "errorHandling/syntaxErrors/InvalidExpression.h"
#include "errorHandling/syntaxErrors/MissingIdentifier.h"
#include "errorHandling/syntaxErrors/MissingSemicolon.h"
#include "errorHandling/syntaxErrors/UnexpectedToken.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), len(tokens.size()), pos(0), symTable(SymbolTable())
{
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
    std::string result;

    for (const auto& stmt : stmts)
    {
        result += stmt->translateToCpp();
    }

    return result;
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

    std::unique_ptr<Expr> expr = parseExpr();
    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));
    symTable.addVar(varType, identifierToken); // to avoid degree x = x + 1║ ...
    return std::make_unique<VarDecStmt>(true, std::move(expr), var);
}

std::unique_ptr<AssignmentStmt> Parser::parseAssignmentStmt()
{
    std::unique_ptr<VarCallExpr> var = parseVarCallExpr();

    const std::wstring op = expectAndGet(Token::OP_ASSIGNMENT).value;

    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));

    return std::make_unique<AssignmentStmt>(std::move(var), op, parseExpr());
}

std::unique_ptr<Expr> Parser::parseExpr()
{
    if (match(Token::CONST_BOOL) || match(Token::CONST_STR) || match(Token::CONST_INT) || match(Token::CONST_FLOAT) || match(Token::CONST_CHAR) )
    {
        return parseConstValue();
    }

    if (match(Token::IDENTIFIER))
    {
        return parseVarCallExpr();
    }

    throw UnexpectedToken(current());
}

std::unique_ptr<ConstValueExpr> Parser::parseConstValue()
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
