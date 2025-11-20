#include "Parser.h"

#include <memory>
#include <vector>

#include "errorHandling/lexicalErrors/InvalidIdentifier.h"
#include "errorHandling/lexicalErrors/UnexpectedEOF.h"
#include "errorHandling/syntaxErrors/UnexpectedToken.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), len(tokens.size()), pos(0), symTable(SymbolTable())
{
}

std::vector<std::unique_ptr<Stmt>> Parser::parse()
{
    std::vector<std::unique_ptr<Stmt>> stmts;

    while (!isAtEnd())
    {
        if (match(Token::TYPE))
        {
            stmts.push_back(parseVarDecStmt());
        }
        else if (match(Token::IDENTIFIER) && peek().type == Token::OP_ASSIGNMENT)
        {
            stmts.push_back(parseAssignmentStmt());
        }
    }

    return stmts;
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
    return pos + 1 >= len;
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

std::unique_ptr<VarDecStmt> Parser::parseVarDecStmt()
{
    expect(Token::TYPE);
    const Type varType(prev().value);
    expect(Token::IDENTIFIER);
    const std::wstring varName = prev().value;

    const Var var(varType, varName);

    symTable.addVar(varType, prev());

    if (match(Token::PUNCTUATION, L"║"))
    {
        return std::make_unique<VarDecStmt>(false, nullptr, var);
    }

    expect(Token::OP_ASSIGNMENT, L"=");

    return std::make_unique<VarDecStmt>(true, parseExpr(), var);
}

std::unique_ptr<AssignmentStmt> Parser::parseAssignmentStmt()
{
    std::unique_ptr<VarCallExpr> var = parseVarCallExpr();

    expect(Token::OP_ASSIGNMENT);
    const std::wstring op = prev().value;

    expect(Token::PUNCTUATION, L"║");

    return std::make_unique<AssignmentStmt>(std::move(var), op, parseExpr());
}

std::unique_ptr<Expr> Parser::parseExpr()
{
    if (match(Token::CONST_BOOL) || match(Token::CONST_STR) || match(Token::CONST_INT) || match(Token::CONST_FLOAT) || match(Token::CONST_CHAR) )
    {
        return parseConstValue();
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
        throw UnexpectedToken(t);
    }

    advance();
    return std::make_unique<ConstValueExpr>(Type(type), t.value);
}

std::unique_ptr<VarCallExpr> Parser::parseVarCallExpr()
{
    expect(Token::IDENTIFIER);
    const std::optional<Var> var = symTable.getVar(prev().value);

    if (!var.has_value())
    {
        throw InvalidIdentifier(prev());
    }

    return std::make_unique<VarCallExpr>(var.value());
}
