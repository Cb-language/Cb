#include "Parser.h"

#include <memory>
#include <sstream>
#include <vector>

#include "AST/statements/FuncDeclStmt.h"
#include "AST/statements/expression/BinaryOpExpr.h"
#include "AST/statements/expression/FuncCallExpr.h"
#include "errorHandling/lexicalErrors/IdentifierTaken.h"
#include "errorHandling/lexicalErrors/InvalidIdentifier.h"
#include "errorHandling/lexicalErrors/InvalidMainArgs.h"
#include "errorHandling/lexicalErrors/InvalidMainReturnType.h"
#include "errorHandling/lexicalErrors/InvalidNumberLiteral.h"
#include "errorHandling/lexicalErrors/MainOverride.h"
#include "errorHandling/lexicalErrors/NoMain.h"
#include "errorHandling/lexicalErrors/UnexpectedEOF.h"
#include "errorHandling/syntaxErrors/ElseIfWithoutIf.h"
#include "errorHandling/syntaxErrors/InvalidExpression.h"
#include "errorHandling/syntaxErrors/MissingBrace.h"
#include "errorHandling/syntaxErrors/MissingIdentifier.h"
#include "errorHandling/syntaxErrors/MissingSemicolon.h"
#include "errorHandling/syntaxErrors/NoReturnStmt.h"
#include "errorHandling/syntaxErrors/UnexpectedToken.h"
#include "errorHandling/syntaxErrors/WrongReturnType.h"


Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), len(tokens.size()), pos(0), symTable(SymbolTable()), hasMain(false)
{
    Utils::init(&symTable);
}

Parser::~Parser()
{
    Utils::reset();
}

void Parser::parse()
{
    auto block = parseBodyStmt({},true);
    for (auto& stmt : block->getStmts())
    {
        stmts.push_back(std::move(stmt));
    }
}

bool Parser::checkLegal()
{
    if (!hasMain)
    {
        throw NoMain(tokens[len-1]);
    }

    while (!creditsQ.empty())
    {
        if (!symTable.isLegalCredit(creditsQ.front()))
        {
            return false;
        }

        creditsQ.pop();
    }

    while (!callsQ.empty())
    {
        FuncCallExpr* call = callsQ.front();
        std::unique_ptr<IType> t = symTable.getCallType(call);

        if (t == nullptr)
        {
            return false;
        }

        call->setType(std::move(t));

        callsQ.pop();
    }

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
    oss << "#include \"includes/Array.h\"" << std::endl;

    oss << std::endl << symTable.getFuncsHeaders();

    for (const auto& stmt : stmts)
    {
        oss << std::endl << stmt->translateToCpp();
    }

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

std::unique_ptr<VarDeclStmt> Parser::parseVarDecStmt()
{
    const std::unique_ptr<IType> varType = parseIType();

    const std::wstring varName = expectAndGet(
        Token::IDENTIFIER, MissingIdentifier(current())
        ).value;

    const Var var(varType->copy(), varName);
    const Token identifierToken = prev();

    if (match(Token::PUNCTUATION, L"║"))
    {
        symTable.addVar(varType->copy(), identifierToken);
        advance();
        return std::make_unique<VarDeclStmt>(symTable.getCurrScope(), symTable.getCurrFunc(), false, nullptr, var);
    }

    expect(Token::OP_ASSIGNMENT, L"=");

    auto expr = parseExpr();
    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));
    symTable.addVar(varType->copy(), identifierToken); // to avoid degree x = x + 1║ ...
    return std::make_unique<VarDeclStmt>(symTable.getCurrScope(), symTable.getCurrFunc(), true, std::move(expr), var);
}

std::unique_ptr<AssignmentStmt> Parser::parseAssignmentStmt()
{
    auto var = parseVarCallExpr();

    const std::wstring op = expectAndGet(Token::OP_ASSIGNMENT).value;


    auto expr = parseExpr();
    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));

    return std::make_unique<AssignmentStmt>(symTable.getCurrScope(), symTable.getCurrFunc(), std::move(var), op, std::move(expr));
}

std::unique_ptr<HearStmt> Parser::parseHearStmt()
{
    std::vector<std::unique_ptr<VarCallExpr>> vars;
    expect(Token::KEYWORD, L"hear");
    expect(
        Token::PUNCTUATION, L"(", MissingBrace(current())
        );

    while (!match(Token::PUNCTUATION, L")"))
    {
        vars.push_back(parseVarCallExpr());

        if (match(Token::PUNCTUATION, L")"))
        {
            break;
        }

        expect(
            Token::PUNCTUATION, L",", InvalidExpression(current())
            );
    }
    advance(); // consume ')'

    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));

    return std::make_unique<HearStmt>(symTable.getCurrScope(), symTable.getCurrFunc(), std::move(vars));
}

std::unique_ptr<PlayStmt> Parser::parsePlayStmt()
{
    bool newline = false;
    std::vector<std::unique_ptr<Expr>> args;

    if (match(Token::KEYWORD, L"play"))
    {
        advance();
    }
    else if (match(Token::KEYWORD, L"playBar"))
    {
        newline = true;
        advance();
    }
    else
    {
        throw UnexpectedToken(current());
    }

    expect(Token::PUNCTUATION, L"(", MissingBrace(current()));

    while (!match(Token::PUNCTUATION, L")"))
    {
        args.push_back(parseExpr());

        if (match(Token::PUNCTUATION, L")"))
        {
            break;
        }

        expect(Token::PUNCTUATION, L",", InvalidExpression(prev()));
    }
    advance(); // consume ')'

    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));

    return std::make_unique<PlayStmt>(symTable.getCurrScope(), symTable.getCurrFunc(), args, newline);
}

std::unique_ptr<BodyStmt> Parser::parseBodyStmt(const std::vector<std::pair<Var, const Token>>& args, const bool isGlobal)
{
    // Enter new scope only if not global
    if (!isGlobal)
    {
        expect(Token::PUNCTUATION, L"∮", MissingBrace(current()));
        symTable.enterScope();

        if (!args.empty())
        {
            for (const auto& p : args)
            {
                symTable.addVar(p.first, p.second);
            }
        }
    }

    std::vector<std::unique_ptr<Stmt>> bodyStmts;

    // Loop until closing brace
    while (!isAtEnd() && !match(Token::PUNCTUATION, L"☉")) // if global, until EOF, otherwise, until ☉
    {
        if (match(Token::TYPE))
        {
            bodyStmts.push_back(parseVarDecStmt());
        }
        else if (match(Token::IDENTIFIER) &&
                 (peek().type == Token::OP_ASSIGNMENT ||
                  (peek().type == Token::PUNCTUATION && peek().value == L"║")))
        {
            bodyStmts.push_back(parseAssignmentStmt());
        }
        else if (match(Token::KEYWORD, L"hear"))
        {
            bodyStmts.push_back(parseHearStmt());
        }
        else if (match(Token::KEYWORD, L"play") ||
                 match(Token::KEYWORD, L"playBar"))
        {
            bodyStmts.push_back(parsePlayStmt());
        }
        else if (match(Token::IDENTIFIER) && peek().type == Token::OP_UNARY)
        {
            bodyStmts.push_back(parseUnaryOpExpr(true));
        }
        else if (match(Token::KEYWORD, L"song"))
        {
            bodyStmts.push_back(parseFuncDeclStmt());
        }
        else if (match(Token::KEYWORD, L"B"))
        {
            bodyStmts.push_back(parseReturnStmt());
        }
        else if (match(Token::IDENTIFIER) && peek().type == Token::PUNCTUATION && peek().value == L"(")
        {
            bodyStmts.push_back(parseFuncCallExpr(true));
        }
        else if (match(Token::KEYWORD, L"D"))
        {
            bodyStmts.push_back(parseIfStmt());
        }
        else if (match(Token::KEYWORD, L"E"))
        {   // checking if there was an if or an else if before this
            if (bodyStmts.front().get()) // TODO fix if statement to check if last one is not if or else if
            {
                throw ElseIfWithoutIf(current());
            }
            bodyStmts.push_back(parseElseIfStmt());
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

    // Consume closing brace
    if (!isGlobal)
    {
        if (isAtEnd())
        {
            throw MissingBrace(prev());
        }

        expect(Token::PUNCTUATION, L"☉", MissingBrace(current()));
        symTable.exitScope();
    }

    return std::make_unique<BodyStmt>(symTable.getCurrScope(), symTable.getCurrFunc(), bodyStmts, isGlobal);
}

std::unique_ptr<FuncDeclStmt> Parser::parseFuncDeclStmt()
{
    std::vector<std::pair<Var, const Token>> args;
    std::vector<std::unique_ptr<FuncCreditStmt>> credited;
    FuncDeclStmt* currFunc = symTable.getCurrFunc();

    expect(Token::KEYWORD, L"song");

    if (match(Token::PUNCTUATION, L"©")) // all functions giving copyrights to
    {
        advance();
        expect(Token::PUNCTUATION, L"(", MissingBrace(current()));
        while (!match(Token::PUNCTUATION, L")"))
        {
            credited.push_back(parseFuncCreditStmt());

            if (!match(Token::PUNCTUATION, L")"))
            {
                expect(Token::PUNCTUATION, L",", UnexpectedToken(current()));
            }
        }
        advance(); // matched the closing brace now moving forward
    }

    const std::wstring funcName = expectAndGet(Token::IDENTIFIER, MissingIdentifier(current())).value;

    if (funcName == L"prelude" && hasMain)
    {
        throw MainOverride(current());
    }

    if (symTable.doesFuncExist(funcName))
    {
        throw IdentifierTaken(current());
    }

    expect(Token::PUNCTUATION, L"(");
    while (!match(Token::PUNCTUATION, L")"))
    {
        const std::unique_ptr<IType> type = parseIType();
        std::wstring currName = expectAndGet(Token::IDENTIFIER, MissingBrace(current())).value;

        args.push_back({Var(type->copy(), currName), current()});

        if (!match(Token::PUNCTUATION, L")"))
        {
            expect(Token::PUNCTUATION, L",", UnexpectedToken(current()));
        }
    }
    advance(); // matched the closing brace now moving forward

    std::vector<Var> varArgs;

    if (!args.empty())
    {
        for (const auto& p : args)
        {
            varArgs.emplace_back(p.first.copy());
        }
    }

    if (!match(Token::PUNCTUATION, L"->"))
    {
        if (funcName == L"prelude")
        {
            throw(InvalidMainReturnType(current())); // if it gets in here, main is void -> err
        }

        auto funcDeclStmt = std::make_unique<FuncDeclStmt>(symTable.getCurrScope(), funcName, std::make_unique<Type>(L"fermata"), varArgs, credited);
        symTable.addFunc(funcDeclStmt->getFunc());
        symTable.changeFunc(funcDeclStmt.get());
        funcDeclStmt->setBody(parseBodyStmt(args));
        symTable.changeFunc(currFunc);
        return std::move(funcDeclStmt);
    }
    advance(); // matched the arrow now moving forward

    const std::unique_ptr<IType> rType = parseIType();

    if (funcName == L"prelude")
    {
        if (!args.empty())
        {
            throw(InvalidMainArgs(current()));
        }

        if (rType->getType() != L"degree")
        {
            throw(InvalidMainReturnType(current()));
        }

        hasMain = true;
    }


    auto funcDeclStmt = std::make_unique<FuncDeclStmt>(symTable.getCurrScope(), funcName, rType->copy(), varArgs, credited);

    symTable.addFunc(funcDeclStmt->getFunc());
    symTable.changeFunc(funcDeclStmt.get());

    funcDeclStmt->setBody(std::move(parseBodyStmt(args)));

    if (!funcDeclStmt->getHasReturned())
    {
        throw(NoReturnStmt(prev()));
    }

    symTable.changeFunc(currFunc);
    return std::move(funcDeclStmt);
}

std::unique_ptr<ReturnStmt> Parser::parseReturnStmt()
{
    expect(Token::KEYWORD, L"B");

    FuncDeclStmt* currFunc = symTable.getCurrFunc();
    std::unique_ptr<Expr> expr = nullptr;

    if (currFunc->getReturnType()->getType() != L"fermata")
    {
        expect(Token::PUNCTUATION, L"\\");
        expr = parseExpr();
        if (*(currFunc->getReturnType()) != *(expr->getType()))
        {
            throw WrongReturnType(current());
        }
    }

    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));

    currFunc->setHasReturned(true);

    return std::make_unique<ReturnStmt>(symTable.getCurrScope(), currFunc, expr);
}

std::unique_ptr<FuncCreditStmt> Parser::parseFuncCreditStmt()
{
    FuncCredit credit(
            expectAndGet(Token::IDENTIFIER, MissingIdentifier(current())).value,current()
            );

    creditsQ.push(credit);

    return std::make_unique<FuncCreditStmt>(symTable.getCurrScope(),symTable.getCurrFunc(), credit);
}

std::unique_ptr<IfStmt> Parser::parseIfStmt()
{
    expect(Token::KEYWORD, L"D");
    expect(Token::PUNCTUATION, L"|", MissingSemicolon(current()));
    std::unique_ptr<Expr> e = parseExpr();
    expect(Token::PUNCTUATION, L"|", MissingSemicolon(current()));
    std::vector<std::pair<Var, const Token>> args; // args is empty
    std::unique_ptr<BodyStmt> b = parseBodyStmt(args, false);
    return std::make_unique<IfStmt>(symTable.getCurrScope(), symTable.getCurrFunc(), e, b);
}

std::unique_ptr<ElseIfStmt> Parser::parseElseIfStmt()
{
    expect(Token::KEYWORD, L"E");
    expect(Token::PUNCTUATION, L"/");
    expect(Token::KEYWORD, L"D");
    expect(Token::PUNCTUATION, L"|", MissingSemicolon(current()));
    std::unique_ptr<Expr> e = parseExpr();
    expect(Token::PUNCTUATION, L"|", MissingSemicolon(current()));
    std::vector<std::pair<Var, const Token>> args; // args is empty
    std::unique_ptr<BodyStmt> b = parseBodyStmt(args, false);
    return std::make_unique<ElseIfStmt>(symTable.getCurrScope(), symTable.getCurrFunc(), e, b);
}

std::unique_ptr<FuncCallExpr> Parser::parseFuncCallExpr(const bool isStmt)
{
    const std::wstring name = expectAndGet(Token::IDENTIFIER, MissingIdentifier(current())).value;
    std::vector<std::unique_ptr<Expr>> args;

    expect(Token::PUNCTUATION, L"(", MissingBrace(current()));

    bool first = true;
    while (!match(Token::PUNCTUATION, L")"))
    {
        if (!first)
        {
            expect(Token::PUNCTUATION, L",");
        }
        first = false;
        args.push_back(parseExpr());
    }
    advance();

    if (isStmt)
    {
        expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));
    }

    auto expr = std::make_unique<FuncCallExpr>(symTable.getCurrScope(), symTable.getCurrFunc(), name, std::move(args), isStmt);

    callsQ.push(expr.get());

    return expr;
}

std::unique_ptr<IType> Parser::parseIType()
{
    return parseType(); // until arrays
}

std::unique_ptr<Type> Parser::parseType()
{
    std::wstring value = expectAndGet(Token::TYPE, InvalidNumberLiteral(current())).value;

    return std::make_unique<Type>(value);
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
    {
        expr->setHasParens(true);
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parsePrimary()
{
    if (current().isConst())
    {
        return parseConstValueExpr();
    }

    if (match(Token::IDENTIFIER))
    {
        if (peek().type == Token::PUNCTUATION && peek().value == L"(")
        {
            return parseFuncCallExpr();
        }
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

        left = std::make_unique<BinaryOpExpr>(symTable.getCurrScope(), symTable.getCurrFunc(), op, std::move(left), std::move(right));
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
    return std::make_unique<ConstValueExpr>(symTable.getCurrScope(), symTable.getCurrFunc(), std::make_unique<Type>(type), t.value);
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

    return std::make_unique<VarCallExpr>(symTable.getCurrScope(), symTable.getCurrFunc(), var.value());
}

std::unique_ptr<UnaryOpExpr> Parser::parseUnaryOpExpr(const bool isStmt)
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
    else if (value == L"!")
    {
        op = UnaryOp::Not;
    }
    else
    {
        throw UnexpectedToken(current());
    }

    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));

    return std::make_unique<UnaryOpExpr>(symTable.getCurrScope(), symTable.getCurrFunc(), std::move(expr), op, isStmt);
}