#include "Parser.h"

#include <memory>
#include <ranges>
#include <sstream>
#include <vector>
#include <filesystem>
#include <algorithm>

// ---------- AST related ----------
#include "AST/statements/FuncDeclStmt.h"
#include "AST/statements/expression/BinaryOpExpr.h"
#include "AST/statements/expression/FuncCallExpr.h"

// ---------- syntax errors ----------
#include "../errorHandling/syntaxErrors/UnexpectedToken.h"
#include "../errorHandling/syntaxErrors/ExpectedAnExpression.h"
#include "../errorHandling/syntaxErrors/MissingBrace.h"
#include "../errorHandling/syntaxErrors/MissingPipe.h"
#include "../errorHandling/syntaxErrors/MissingSemicolon.h"
#include "../errorHandling/syntaxErrors/NoPlacementOperator.h"
#include "../errorHandling/syntaxErrors/InvalidExpression.h"
#include "../errorHandling/syntaxErrors/StmtInsideSwitchThatIsNotCase.h"
#include "../errorHandling/syntaxErrors/MissingIdentifier.h"
#include "../errorHandling/syntaxErrors/MissingParenthesis.h"
#include "../errorHandling/syntaxErrors/IncludeNotInTop.h"
#include "../errorHandling/syntaxErrors/ExpectedAPath.h"

// ---------- semantic errors ----------
#include "../errorHandling/semanticErrors/IdentifierTaken.h"
#include "../errorHandling/semanticErrors/StmtNotBreakable.h"
#include "../errorHandling/semanticErrors/StmtNotContinueAble.h"
#include "../errorHandling/semanticErrors/NoReturn.h"
#include "../errorHandling/semanticErrors/WrongReturnType.h"
#include "../errorHandling/semanticErrors/UnrecognizedIdentifier.h"
#include "../errorHandling/semanticErrors/IllegalCredit.h"
#include "../errorHandling/semanticErrors/IllegalCall.h"
#include "../errorHandling/semanticErrors/InvalidPathExtension.h"

// ---------- entry point errors ----------
#include "../errorHandling/entryPointErrors/InvalidMainReturnType.h"
#include "../errorHandling/entryPointErrors/InvalidMainArgs.h"
#include "../errorHandling/entryPointErrors/MainOverride.h"
#include "../errorHandling/entryPointErrors/NoMain.h"

// ---------- lexical errors ----------
#include "../errorHandling/lexicalErrors/InvalidNumberLiteral.h"
#include "../errorHandling/lexicalErrors/UnexpectedEOF.h"
#include "../errorHandling/lexicalErrors/UnrecognizedToken.h"

// ---------- just how ----------
#include "../errorHandling/how/HowDidYouGetHere.h"

#include "errorHandling/classErrors/MissingClassPipe.h"
#include "errorHandling/classErrors/NoCtor.h"

#include "files/FileGraph.h"


Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), len(tokens.size()), pos(0), hasMain(false), symTable(SymbolTable())
{
}

Parser::~Parser() = default;

std::vector<std::pair<std::filesystem::path, Token>> Parser::readIncludes()
{
    std::vector<std::pair<std::filesystem::path, Token>> v;
    if (includes.empty() && pos == 0)
    {
        while (match(Token::KEYWORD, L"feat"))
        {
            advance();
            Token pathToken = expectAndGet(Token::CONST_STR, ExpectedAPath(current()));
            std::wstring wstr = pathToken.value;
            wstr.erase(
                std::ranges::remove(wstr, L'"').begin(),
                wstr.end()
            );
            std::filesystem::path path = wstr;
            expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));

            if (path.extension() != ".cb")
            {
                throw InvalidPathExtension(current());
            }

            includes.emplace_back(std::make_unique<IncludeStmt>(pathToken, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), path));
            v.emplace_back(path, pathToken);
        }
    }

    return v;
}

void Parser::parse()
{
    auto block = parseBodyStmt({},true);
    for (auto& stmt : block->getStmts())
    {
        stmts.push_back(std::move(stmt));
    }
}

void Parser::analyze()
{
    while (!creditsQ.empty())
    {
        if (!symTable.isLegalCredit(creditsQ.front()))
        {
            throw IllegalCredit(creditsQ.front().getToken(), Utils::wstrToStr(creditsQ.front().getName()));
        }

        creditsQ.pop();
    }

    while (!callsQ.empty())
    {
        FuncCallExpr* call = callsQ.front();
        std::unique_ptr<IType> t = symTable.getCallType(call);

        if (t == nullptr)
        {
            throw IllegalCall(call->getToken(), Utils::wstrToStr(call->getName()));
        }

        call->setType(std::move(t));

        callsQ.pop();
    }

    for (const auto& stmt : stmts)
    {
        stmt->analyze();
    }
}

std::string Parser::translateToCpp(const std::filesystem::path& hPath, const bool isMain) const
{
    std::ostringstream oss;

    oss << "#include <iostream>" << std::endl;

    if (isMain) oss << translateToH(isMain);
    else oss << "#include \"" << hPath.string() << "\"" << std::endl;

    for (const auto& stmt : stmts)
    {
        oss << std::endl << stmt->translateToCpp();
    }

    return oss.str();
}

std::string Parser::translateToH(const bool isMain) const
{
    std::ostringstream oss;
    const std::filesystem::path outArrPath = File::getOutDir() / "includes" / "Array.h";
    if (!isMain) oss << "#pragma once" << std::endl;

    oss << "#include <string>" << std::endl;
    if (isMain) oss << "#include \"" << outArrPath.string() << "\"" << std::endl;

    if (!includes.empty()) oss << std::endl;

    for (const auto& i : includes)
    {
        oss << i->translateToCpp() << std::endl;
    }

    for (const auto& stmt : stmts) oss << stmt->translateToH();

    return oss.str();
}

void Parser::addToSymTable(const SymbolTable& symTable)
{
    this->symTable += symTable;
}

const SymbolTable& Parser::getSymTable() const
{
    return symTable;
}

bool Parser::getHasMain() const
{
    return hasMain;
}

const Token& Parser::getLast() const
{
    return tokens[len - 1];
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
    if (isAtEnd()) throw UnexpectedEOF(tokens[len-1]);
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

bool Parser::isAssignmentStmtAhead()
{
    const size_t startPos = pos;

    // Must start with identifier
    if (!match(Token::IDENTIFIER))
    {
        return false;
    }

    advance(); // consume IDENTIFIER

    // Skip any number of indexing / slicing brackets
    while (match(Token::PUNCTUATION, L"["))
    {
        advance(); // consume '['

        int depth = 1;
        while (depth > 0)
        {
            if (match(Token::PUNCTUATION, L"["))
            {
                depth++;
            }
            else if (match(Token::PUNCTUATION, L"]"))
            {
                depth--;
            }

            advance();
        }
    }

    const bool result =
        match(Token::OP_ASSIGNMENT) ||
        match(Token::PUNCTUATION, L"║");

    pos = startPos;
    return result;
}

bool Parser::isUnaryOpStmtAhead()
{
    const size_t startPos = pos;

    if (!match(Token::IDENTIFIER))
    {
        return false;
    }

    advance(); // consume IDENTIFIER

    // Skip indexing / slicing
    while (match(Token::PUNCTUATION, L"["))
    {
        advance();

        int depth = 1;
        while (depth > 0)
        {
            if (match(Token::PUNCTUATION, L"["))
            {
                depth++;
            }
            else if (match(Token::PUNCTUATION, L"]"))
            {
                depth--;
            }
            advance();
        }
    }

    bool result = match(Token::OP_UNARY);

    pos = startPos;
    return result;
}

std::unique_ptr<VarDeclStmt> Parser::parseVarDecStmt(const bool isField)
{
    if (match(Token::TYPE, L"riff"))
    {
        return parseArrayDeclStmt();
    }

    const Token& t = current();
    const std::unique_ptr<IType> varType = parseIType();

    const std::wstring varName = expectAndGet(
        Token::IDENTIFIER, MissingIdentifier(current())
        ).value;

    const Var var(varType->copy(), varName);
    const Token identifierToken = prev();

    if (isField && !match(Token::OP_ASSIGNMENT, L"="))
    {
        symTable.addVar(varType->copy(), identifierToken);
        return std::make_unique<VarDeclStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), false, nullptr, var);
    }

    if (match(Token::PUNCTUATION, L"║"))
    {
        symTable.addVar(varType->copy(), identifierToken);
        advance();
        return std::make_unique<VarDeclStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), false, nullptr, var);
    }

    expect(Token::OP_ASSIGNMENT, L"=" , NoPlacementOperator(current()));

    auto expr = parseExpr();

    if (!isField)
    {
        expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));
        symTable.addVar(varType->copy(), identifierToken); // to avoid degree x = x + 1║ ...
    }

    return std::make_unique<VarDeclStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), true, std::move(expr), var);
}

std::unique_ptr<AssignmentStmt> Parser::parseAssignmentStmt()
{
    const Token& t = current();
    std::unique_ptr<Call> call = parseCallExpr();

    const std::wstring op = expectAndGet(Token::OP_ASSIGNMENT).value;


    auto expr = parseExpr();
    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));

    return std::make_unique<AssignmentStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(call), op, std::move(expr));
}

std::unique_ptr<HearStmt> Parser::parseHearStmt()
{
    std::vector<std::unique_ptr<Call>> calls;
    const Token& t = current();
    expect(Token::KEYWORD, L"hear", HowDidYouGetHere(current()));
    expect(
        Token::PUNCTUATION, L"(", MissingBrace(current())
        );

    while (!match(Token::PUNCTUATION, L")"))
    {
        calls.push_back(parseCallExpr());

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

    return std::make_unique<HearStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), calls);
}

std::unique_ptr<PlayStmt> Parser::parsePlayStmt()
{
    bool newline = false;
    std::vector<std::unique_ptr<Expr>> args;
    const Token& t = current();

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
        throw HowDidYouGetHere(current());
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

    return std::make_unique<PlayStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(args), newline);
}

std::unique_ptr<BodyStmt> Parser::parseBodyStmt(const std::vector<std::pair<Var, const Token>>& args, const bool isGlobal, const bool isBreakable, const bool isContinueAble, const bool hasBrace)
{
    // Enter new scope only if not global
    const Token& t = current();
    if (!isGlobal && hasBrace)
    {
        expect(Token::PUNCTUATION, L"∮", MissingBrace(current()));
        symTable.enterScope(isBreakable, isContinueAble);

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
        else if (match(Token::IDENTIFIER) && isAssignmentStmtAhead())
        {
            if (symTable.getVar(current().value).value().isPrimitive())
            {
                bodyStmts.push_back(parseAssignmentStmt());
            }
            else
            {
                // TODO add ObjInstance parsing
            }
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
        else if (isUnaryOpStmtAhead())
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
        else if (match(Token::KEYWORD, L"G"))
        {
            bodyStmts.push_back((parseWhileStmt()));
        }
        else if (match(Token::KEYWORD, L"A"))
        {
            bodyStmts.push_back(parseSwitchStmt());
        }
        else if (match(Token::KEYWORD, L"pause"))
        {
            if (!symTable.getCurrScope()->getIsBreakable())
            {
                throw StmtNotBreakable(current());
            }
            bodyStmts.push_back(parseBreakStmt());
            if (!hasBrace)
            {
                break;
            }
        }
        else if (match(Token::KEYWORD, L"resume"))
        {
            if (!symTable.getCurrScope()->getIsContinueAble())
            {
                throw StmtNotContinueAble(current());
            }
            bodyStmts.push_back(parseContinueStmt());
            if (!hasBrace)
            {
                break;
            }
        }
        else if (match(Token::KEYWORD, L"Fmaj") || match(Token::KEYWORD, L"Fmin"))
        {
            bodyStmts.push_back(parseForStmt());
        }
        else if (match(Token::KEYWORD, L"instrument"))
        {
            bodyStmts.push_back(parseClassDeclStmt());
        }
        else if (match(Token::COMMENT_MULTI) || match(Token::COMMENT_SINGLE))
        {
            advance();
        }
        else if (symTable.isClass(current().value))
        {
            bodyStmts.push_back(parseObjCreationStmt());
        }
        else if (match(Token::KEYWORD, L"feat"))
        {
            throw IncludeNotInTop(current());
        }
        else
        {
            throw UnrecognizedToken(current());
        }
    }

    // Consume closing brace
    if (!isGlobal && hasBrace)
    {
        if (isAtEnd())
        {
            throw MissingBrace(prev());
        }

        // this is an if and not expect because it would crash if the brace is the end of the file
        if (!match(Token::PUNCTUATION, L"☉"))
        {
            throw MissingBrace(current());
        }
        if (!isAtEnd())
        {
            advance();
        }
        symTable.exitScope();
    }

    return std::make_unique<BodyStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), bodyStmts, isGlobal);
}

std::unique_ptr<FuncDeclStmt> Parser::parseFuncDeclStmt(const bool isMethod)
{
    std::vector<std::pair<Var, const Token>> args;
    std::vector<std::unique_ptr<FuncCreditStmt>> credited;
    IFuncDeclStmt* currFunc = symTable.getCurrFunc();
    const Token& t = current();

    expect(Token::KEYWORD, L"song", HowDidYouGetHere(current()));

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

    expect(Token::PUNCTUATION, L"(", MissingParenthesis(current()));
    while (!match(Token::PUNCTUATION, L")"))
    {
        const std::unique_ptr<IType> type = parseIType();
        std::wstring currName = expectAndGet(Token::IDENTIFIER, MissingBrace(current())).value;

        args.emplace_back(Var(type->copy(), currName), current());

        if (!match(Token::PUNCTUATION, L")"))
        {
            expect(Token::PUNCTUATION, L",", UnexpectedToken(current()));
        }
    }
    advance(); // matched the closing brace now moving forward

    std::vector<Var> varArgs;

    if (!args.empty())
    {
        for (const auto& key : args | std::views::keys)
        {
            varArgs.emplace_back(key.copy());
        }
    }

    if (!match(Token::PUNCTUATION, L"->"))
    {
        if (funcName == L"prelude")
        {
            throw(InvalidMainReturnType(current())); // if it gets in here, main is void -> err
        }

        auto funcDeclStmt = std::make_unique<FuncDeclStmt>(t, symTable.getCurrScope(), funcName, std::make_unique<Type>(L"fermata"), varArgs, credited);
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


    auto funcDeclStmt = std::make_unique<FuncDeclStmt>(t, symTable.getCurrScope(), funcName, rType->copy(), varArgs, credited);

    if (!isMethod) symTable.addFunc(funcDeclStmt->getFunc());
    symTable.changeFunc(funcDeclStmt.get());

    funcDeclStmt->setBody(std::move(parseBodyStmt(args)));

    if (!funcDeclStmt->getHasReturned())
    {
        throw(NoReturn(prev()));
    }

    symTable.changeFunc(currFunc);
    return std::move(funcDeclStmt);
}

std::unique_ptr<ReturnStmt> Parser::parseReturnStmt()
{
    const Token& t = current();
    expect(Token::KEYWORD, L"B", HowDidYouGetHere(current()));

    IFuncDeclStmt* currFunc = symTable.getCurrFunc();
    std::unique_ptr<Expr> expr = nullptr;

    if (currFunc->getReturnType()->getType() != L"fermata")
    {
        expect(Token::PUNCTUATION, L"\\", ExpectedAnExpression(current()));
        expr = parseExpr();
        if (*(currFunc->getReturnType()) != *(expr->getType()))
        {
            throw WrongReturnType(current());
        }
    }

    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));

    currFunc->setHasReturned(true);

    return std::make_unique<ReturnStmt>(t, symTable.getCurrScope(), currFunc, expr);
}

std::unique_ptr<FuncCreditStmt> Parser::parseFuncCreditStmt()
{
    const Token& t = current();
    FuncCredit credit(
            expectAndGet(Token::IDENTIFIER, MissingIdentifier(current())).value,current()
            );

    creditsQ.push(credit);

    return std::make_unique<FuncCreditStmt>(t, symTable.getCurrScope(),symTable.getCurrFunc(), credit);
}

std::unique_ptr<IfStmt> Parser::parseIfStmt()
{
    const Token& t = current();
    expect(Token::KEYWORD, L"D", HowDidYouGetHere(current()));
    expect(Token::PUNCTUATION, L"|", MissingPipe(current()));
    std::unique_ptr<Expr> expr = parseExpr();
    expect(Token::PUNCTUATION, L"|", MissingPipe(current()));
    constexpr std::vector<std::pair<Var, const Token>> args; // args is empty
    std::unique_ptr<Stmt> body = parseBodyStmt(args, false);

    if (match(Token::KEYWORD, L"E"))
    {
        advance();
        if (match(Token::PUNCTUATION, L"\\"))
        {
            advance();
            return std::make_unique<IfStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(expr), std::move(body), parseIfStmt(), true);
        }
        return std::make_unique<IfStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(expr), std::move(body), parseBodyStmt(args, false), false);
    }
    return std::make_unique<IfStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(expr), std::move(body), nullptr, false);
}

std::unique_ptr<ArrayDeclStmt> Parser::parseArrayDeclStmt()
{
    const Token& t = current();
    std::unique_ptr<IType> arrType = parseIType();
    const unsigned int arrLevel = arrType->getArrLevel();
    const std::wstring name = expectAndGet(Token::IDENTIFIER, MissingIdentifier(current())).value;
    std::vector<std::unique_ptr<Expr>> sizes;
    std::unique_ptr<Expr> startExpr = nullptr;

    Var var(std::move(arrType), name);

    symTable.addVar(var.copy(), current());

    for (unsigned int i = 0; i < arrLevel; i++)
    {
        expect(Token::PUNCTUATION, L"[", MissingBrace(current()));

        if (match(Token::PUNCTUATION, L"]"))
        {
            advance();
            sizes.push_back(
                std::make_unique<ConstValueExpr>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::make_unique<Type>(L"degree"), L"1") // default 1
                );
        }
        else
        {
            sizes.push_back(parseExpr());
            expect(Token::PUNCTUATION, L"]", MissingBrace(current()));
        }
    }

    if (match(Token::OP_ASSIGNMENT, L"="))
    {
        advance();
        startExpr = parseExpr();
    }

    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));
    return std::make_unique<ArrayDeclStmt>(
        t,
        symTable.getCurrScope(),
        symTable.getCurrFunc(),
        startExpr != nullptr,
        std::move(startExpr),
        var,
        std::move(sizes)
    );
}

std::unique_ptr<WhileStmt> Parser::parseWhileStmt()
{
    const Token& t = current();
    expect(Token::KEYWORD, L"G", HowDidYouGetHere(current()));
    expect(Token::PUNCTUATION, L"║:", MissingBrace(current()));
    std::unique_ptr<Expr> expr = parseExpr();
    expect(Token::PUNCTUATION, L":║", MissingBrace(current()));
    constexpr std::vector<std::pair<Var, const Token>> args; // args is empty
    std::unique_ptr<Stmt> body = parseBodyStmt(args, false, true, true, true);

    return std::make_unique<WhileStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), expr, body);
}

std::unique_ptr<BreakStmt> Parser::parseBreakStmt()
{
    const Token& t = current();
    expect(Token::KEYWORD, L"pause", HowDidYouGetHere(current()));
    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));

    return std::make_unique<BreakStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass());
}

std::unique_ptr<CaseStmt> Parser::parseCaseStmt()
{
    bool isDefault = false;
    const Token& t = current();

    expect(Token::KEYWORD, L"C");
    std::unique_ptr<Expr> e;
    if (match(Token::PUNCTUATION, L"\\"))
    {
        advance();
        e = parseExpr();
    }
    else
    {
        isDefault = true;
    }
    expect(Token::PUNCTUATION, L"|", MissingPipe(current()));

    symTable.enterScope(true, false);
    std::vector<std::pair<Var, const Token>> args;
    std::unique_ptr<BodyStmt> body = parseBodyStmt(args, false, true, false, false);
    body->setHasBrace(false);

    symTable.exitScope();

    return std::make_unique<CaseStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(e), std::move(body), isDefault);
}

std::unique_ptr<SwitchStmt> Parser::parseSwitchStmt()
{
    const Token& t = current();
    expect(Token::KEYWORD, L"A", HowDidYouGetHere(current()));
    expect(Token::PUNCTUATION, L"\\");
    const std::optional<Var> v = symTable.getCurrScope()->getVar(current().value, symTable.getCurrClass())->copy();

    if (v == std::nullopt)
    {
        throw UnrecognizedIdentifier(current());
    }

    advance();
    expect(Token::PUNCTUATION, L"∮", MissingBrace(current()));
    std::vector<std::unique_ptr<CaseStmt>> cases;

    while (!match(Token::PUNCTUATION, L"☉"))
    {
        if (match(Token::KEYWORD, L"C"))
        {
            cases.push_back(parseCaseStmt());
        }
        else
        {
            throw StmtInsideSwitchThatIsNotCase(current()); // cant do commands in if scope only cases
        }
    }

    expect(Token::PUNCTUATION, L"☉", MissingBrace(current()));

    return std::make_unique<SwitchStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), v.value().copy(), cases);
}

std::unique_ptr<ContinueStmt> Parser::parseContinueStmt()
{
    const Token& t = current();
    expect(Token::KEYWORD, L"resume", MissingBrace(current()));
    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));

    return std::make_unique<ContinueStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass());
}

std::unique_ptr<ForStmt> Parser::parseForStmt()
{
    bool isIncreasing = false;
    std::unique_ptr<Expr> startExpr = nullptr;
    std::unique_ptr<Expr> stopExpr = nullptr;
    std::unique_ptr<Expr> stepExpr = nullptr;
    std::unique_ptr<BodyStmt> body = nullptr;
    std::wstring varName = L"i";
    const Token& t = current();

    if (match(Token::KEYWORD, L"Fmaj"))
    {
        isIncreasing = true;
        advance();
    }
    else
    {
        expect(Token::KEYWORD, L"Fmin", HowDidYouGetHere(current()));
    }

    // ReSharper disable once CppJoinDeclarationAndAssignment
    // order matters
    startExpr = parseExpr();


    if ((isIncreasing && match(Token::OP_UNARY, L"♯"))|| (!isIncreasing && match(Token::OP_UNARY, L"♭")))
    {
        advance();
        stepExpr = parseExpr();
    }

    if (match(Token::PUNCTUATION, L"#"))
    {
        advance();
        stopExpr = parseExpr();
    }

    if (match(Token::PUNCTUATION, L"\\"))
    {
        advance();
        varName = current().value;
        advance();
    }

    std::vector<std::pair<Var, const Token>> args;
    args.emplace_back( Var(std::make_unique<Type>(L"degree"), varName), current());
    body = parseBodyStmt(args, false, true, true, true);

    return std::make_unique<ForStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(),
        std::move(body), isIncreasing, std::move(startExpr), std::move(stepExpr), std::move(stopExpr), varName);
}

std::unique_ptr<ClassDeclStmt> Parser::parseClassDeclStmt()
{
    Token t = current();
    expect(Token::KEYWORD, L"instrument", HowDidYouGetHere(t));

    const std::wstring name = expectAndGet(Token::IDENTIFIER, MissingIdentifier(current())).value;
    std::vector<Field> fields;
    std::vector<Method> methods;
    std::vector<Ctor> ctors;

    Class c(name);
    symTable.setClass(c);

    const size_t tempPos = pos;

    expect(Token::PUNCTUATION, L"∮", MissingBrace(current()));
    parseFields(fields);
    expect(Token::PUNCTUATION, L"☉", MissingBrace(current()));

    pos = tempPos;

    expect(Token::PUNCTUATION, L"∮", MissingBrace(current()));
    parseMethods(methods);
    expect(Token::PUNCTUATION, L"☉", MissingBrace(current()));

    pos = tempPos;

    expect(Token::PUNCTUATION, L"∮", MissingBrace(current()));
    parseCtors(ctors);
    expect(Token::PUNCTUATION, L"☉", MissingBrace(current()));

    symTable.addClass(c);

    return std::make_unique<ClassDeclStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), name, fields, methods, ctors);
}

std::unique_ptr<ConstractorDeclStmt> Parser::parseCtor()
{
    const Token& t = current();
    std::vector<std::pair<Var, const Token>> args;
    std::vector<std::unique_ptr<FuncCreditStmt>> credited;
    IFuncDeclStmt* currFunc = symTable.getCurrFunc();

    expect(Token::IDENTIFIER_CALL, MissingIdentifier(t));

    const std::wstring funcName = t.value;

    std::optional<Class> currClass = symTable.getCurrScope()->getCurrClass();

    if (!currClass.has_value()) throw HowDidYouGetHere(t);

    expect(Token::PUNCTUATION, L"(", MissingParenthesis(current()));
    while (!match(Token::PUNCTUATION, L")"))
    {
        const std::unique_ptr<IType> type = parseIType();
        std::wstring currName = expectAndGet(Token::IDENTIFIER, MissingBrace(current())).value;

        args.emplace_back(Var(type->copy(), currName), current());

        if (!match(Token::PUNCTUATION, L")"))
        {
            expect(Token::PUNCTUATION, L",", UnexpectedToken(current()));
        }
    }
    advance(); // matched the closing brace now moving forward

    std::vector<Var> varArgs;

    if (!args.empty())
    {
        for (const auto& key : args | std::views::keys)
        {
            varArgs.emplace_back(key.copy());
        }
    }

    if (currClass->hasConstractor(Constractor(varArgs, funcName)))
    {
        throw Error(t, "Redefinition of a constractor");
    }

    Scope* s = symTable.getCurrScope();
    auto ctorDeclStmt = std::make_unique<ConstractorDeclStmt>(t, s, s->getCurrClassName(), varArgs);

    // TODO: figure this out
    // symTable.addFunc(ctorDeclStmt->getFunc());
    // symTable.changeFunc(funcDeclStmt.get());

    ctorDeclStmt->setBody(std::move(parseBodyStmt(args)));

    symTable.changeFunc(currFunc);
    return std::move(ctorDeclStmt);
}

std::unique_ptr<ObjCreationStmt> Parser::parseObjCreationStmt()
{
    const Token& classToken = current();

    const std::optional<Class> classInfo = symTable.getClass(classToken.value);
    if (!classInfo)
    {
        throw HowDidYouGetHere(classToken);
    }

    const auto objectType = std::make_unique<Type>(classInfo->getClassName());

    advance();

    const Token& instanceToken = expectAndGet(Token::IDENTIFIER, MissingIdentifier(current()));
    const std::wstring instanceName = instanceToken.value;

    const Var newObjectVar(objectType->copy(), instanceName);

    symTable.addVar(newObjectVar, instanceToken);

    if (match (Token::OP_ASSIGNMENT, L"="))
    {
        if (classInfo->getConstractors().empty())
        {
            throw NoCtor(current());
        }
        if (!parseCrtorCall(classInfo))
        {
            throw Error(current(), "Invalid constructor or parameters");
        }
    }

    expect(Token::PUNCTUATION, L"║", MissingSemicolon(current()));

    return std::make_unique<ObjCreationStmt>(classToken, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), classInfo, instanceName);
}

bool Parser::parseCrtorCall(const std::optional<Class>& _class)
{
    const Token& t = current();
    std::vector<std::pair<Var, const Token>> args;
    IFuncDeclStmt* currFunc = symTable.getCurrFunc();

    expect(Token::IDENTIFIER_CALL, MissingIdentifier(t));
    const std::wstring funcName = t.value;

    if (!_class.has_value()) throw HowDidYouGetHere(t);

    expect(Token::PUNCTUATION, L"(", MissingParenthesis(current()));
    while (!match(Token::PUNCTUATION, L")"))
    {
        const std::unique_ptr<IType> type = parseIType();
        std::wstring currName = expectAndGet(Token::IDENTIFIER, MissingBrace(current())).value;

        args.emplace_back(Var(type->copy(), currName), current());

        if (!match(Token::PUNCTUATION, L")"))
        {
            expect(Token::PUNCTUATION, L",", UnexpectedToken(current()));
        }
    }
    advance(); // matched the closing brace now moving forward

    std::vector<Var> varArgs;

    if (!args.empty())
    {
        for (const auto& key : args | std::views::keys)
        {
            varArgs.emplace_back(key.copy());
        }
    }

    if (_class->hasConstractor(Constractor(varArgs, funcName)))
    {
        return true;
    }
    return false;
}

void Parser::parseFields(std::vector<Field>& fields)
{
    while (!match(Token::PUNCTUATION, L"☉"))
    {
        if (match(Token::KEYWORD, L"playerScore"))
        {
            advance();
            expect(Token::PUNCTUATION, L"∮", MissingBrace(current()));

            while (true)
            {
                if (match(Token::TYPE))
                {
                    auto field = parseVarDecStmt(true);

                    symTable.getCurrScope()->addField(false, field->getVar().copy(), current());

                    fields.emplace_back(false, std::move(field));
                }
                else // skip func decl
                {
                    unsigned int level = 0;
                    while (true)
                    {
                        if (level == 0 && (match(Token::PUNCTUATION, L"|") || match(Token::PUNCTUATION, L"║"))) break;

                        if (match(Token::PUNCTUATION, L"∮")) level++;
                        else if (match(Token::PUNCTUATION, L"☉")) level--;
                        advance();
                    }
                }

                if (match(Token::PUNCTUATION, L"║"))
                {
                    break;
                }

                expect(Token::PUNCTUATION, L"|", MissingClassPipe(current()));
            }
            advance(); // consume the ║
            expect(Token::PUNCTUATION, L"☉", MissingBrace(current()));
        }
        else if (match(Token::KEYWORD, L"conductorScore"))
        {
            advance();
            expect(Token::PUNCTUATION, L"∮", MissingBrace(current()));

            while (true)
            {
                if (match(Token::TYPE))
                {
                    auto field = parseVarDecStmt(true);

                    symTable.getCurrScope()->addField(true, field->getVar().copy(), current());

                    fields.emplace_back(true, std::move(field));
                }
                else // skip func decl
                {
                    unsigned int level = 0;
                    while (true)
                    {
                        if (level == 0 && (match(Token::PUNCTUATION, L"|") || match(Token::PUNCTUATION, L"║"))) break;

                        if (match(Token::PUNCTUATION, L"∮")) level++;
                        else if (match(Token::PUNCTUATION, L"☉")) level--;
                        advance();
                    }
                }

                if (match(Token::PUNCTUATION, L"║"))
                {
                    break;
                }

                expect(Token::PUNCTUATION, L"|", MissingClassPipe(current()));
            }
            advance(); // consume the ║
            expect(Token::PUNCTUATION, L"☉", MissingBrace(current()));
        }
        else
        {
            throw UnrecognizedIdentifier(current());
        }
    }
}

void Parser::parseMethods(std::vector<Method>& methods)
{
    while (!match(Token::PUNCTUATION, L"☉"))
    {
        if (match(Token::KEYWORD, L"playerScore"))
        {
            advance();
            expect(Token::PUNCTUATION, L"∮", MissingBrace(current()));

            while (true)
            {
                if (match(Token::KEYWORD, L"song"))
                {
                    auto method = parseFuncDeclStmt();

                    symTable.getCurrScope()->addMethod(false, method->getFunc().copy(), current());

                    methods.emplace_back(false, std::move(method));
                }
                else
                {
                    unsigned int level = 0;
                    while (true)
                    {
                        if (level == 0 && (match(Token::PUNCTUATION, L"|") || match(Token::PUNCTUATION, L"║"))) break;

                        if (match(Token::PUNCTUATION, L"∮")) level++;
                        else if (match(Token::PUNCTUATION, L"☉")) level--;
                        advance();
                    }
                }

                if (match(Token::PUNCTUATION, L"║"))
                {
                    break;
                }

                expect(Token::PUNCTUATION, L"|", MissingClassPipe(current()));
            }
            advance(); // consume the ║
            expect(Token::PUNCTUATION, L"☉", MissingBrace(current()));
        }
        else if (match(Token::KEYWORD, L"conductorScore"))
        {
            advance();
            expect(Token::PUNCTUATION, L"∮", MissingBrace(current()));

            while (true)
            {
                if (match(Token::KEYWORD, L"song"))
                {
                    auto method = parseFuncDeclStmt();

                    symTable.getCurrScope()->addMethod(false, method->getFunc().copy(), current());

                    methods.emplace_back(true, std::move(method));
                }
                else
                {
                    unsigned int level = 0;
                    while (true)
                    {
                        if (level == 0 && (match(Token::PUNCTUATION, L"|") || match(Token::PUNCTUATION, L"║"))) break;

                        if (match(Token::PUNCTUATION, L"∮")) level++;
                        else if (match(Token::PUNCTUATION, L"☉")) level--;
                        advance();
                    }
                }

                if (match(Token::PUNCTUATION, L"║"))
                {
                    break;
                }

                expect(Token::PUNCTUATION, L"|", MissingClassPipe(current()));
            }
            advance(); // consume the ║
            expect(Token::PUNCTUATION, L"☉", MissingBrace(current()));
        }
        else
        {
            throw UnrecognizedIdentifier(current());
        }
    }
}

void Parser::parseCtors(std::vector<Ctor>& ctors)
{
    while (!match(Token::PUNCTUATION, L"☉"))
    {
        if (match(Token::KEYWORD, L"playerScore"))
        {
            advance();
            expect(Token::PUNCTUATION, L"∮", MissingBrace(current()));

            while (true)
            {
                if (match(Token::IDENTIFIER_CALL))
                {
                    auto ctor = parseCtor();

                    symTable.getCurrScope()->addConstractor(false, ctor->getConstractor().copy(), current());

                    ctors.emplace_back(false, std::move(ctor));
                }
                else // skip func decl
                {
                    unsigned int level = 0;
                    while (true)
                    {
                        if (level == 0 && (match(Token::PUNCTUATION, L"|") || match(Token::PUNCTUATION, L"║"))) break;

                        if (match(Token::PUNCTUATION, L"∮")) level++;
                        else if (match(Token::PUNCTUATION, L"☉")) level--;
                        advance();
                    }
                }

                if (match(Token::PUNCTUATION, L"║"))
                {
                    break;
                }

                expect(Token::PUNCTUATION, L"|", MissingClassPipe(current()));
            }
            advance(); // consume the ║
            expect(Token::PUNCTUATION, L"☉", MissingBrace(current()));
        }
        else if (match(Token::KEYWORD, L"conductorScore"))
        {
            advance();
            expect(Token::PUNCTUATION, L"∮", MissingBrace(current()));

            while (true)
            {
                if (match(Token::IDENTIFIER_CALL))
                {
                    auto ctor = parseCtor();

                    symTable.getCurrScope()->addConstractor(true, ctor->getConstractor().copy(), current());

                    ctors.emplace_back(true, std::move(ctor));
                }
                else // skip func decl
                {
                    unsigned int level = 0;
                    while (true)
                    {
                        if (level == 0 && (match(Token::PUNCTUATION, L"|") || match(Token::PUNCTUATION, L"║"))) break;

                        if (match(Token::PUNCTUATION, L"∮")) level++;
                        else if (match(Token::PUNCTUATION, L"☉")) level--;
                        advance();
                    }
                }

                if (match(Token::PUNCTUATION, L"║"))
                {
                    break;
                }

                expect(Token::PUNCTUATION, L"|", MissingClassPipe(current()));
            }
            advance(); // consume the ║
            expect(Token::PUNCTUATION, L"☉", MissingBrace(current()));
        }
        else
        {
            throw UnrecognizedIdentifier(current());
        }
    }
}

std::unique_ptr<Call> Parser::parseFuncCallExpr(const bool isStmt)
{
    const Token& t = current();
    const std::wstring name = expectAndGet(Token::IDENTIFIER, MissingIdentifier(current())).value;
    std::vector<std::unique_ptr<Expr>> args;

    expect(Token::PUNCTUATION, L"(", MissingParenthesis(current()));

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

    auto expr = std::make_unique<FuncCallExpr>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), name, std::move(args), isStmt);

    callsQ.push(expr.get());

    return expr;
}

std::unique_ptr<Call> Parser::parseCallExpr()
{
    const Token& t = current();
    const std::optional<Var> var = symTable.getVar(
        expectAndGet(Token::IDENTIFIER, MissingIdentifier(current())
            ).value);

    if (!var.has_value())
    {
        throw UnrecognizedIdentifier(prev());
    }

    std::unique_ptr<Call> call = std::make_unique<VarCallExpr>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), var.value().copy());

    if (match(Token::PUNCTUATION, L"["))
    {
        return parseArrayAccess(std::move(call));
    }

    return call;
}

std::unique_ptr<Call> Parser::parseArrayAccess(std::unique_ptr<Call> call)
{
    while (match(Token::PUNCTUATION, L"["))
    {
        const size_t savedPos = pos;

        // peek inside brackets to detect slicing
        bool isSlicing = false;
        size_t lookahead = pos + 1;
        while (lookahead < tokens.size() && !(tokens[lookahead].type == Token::PUNCTUATION && tokens[lookahead].value == L"]"))
        {
            if (tokens[lookahead].type == Token::PUNCTUATION && tokens[lookahead].value == L":")
            {
                isSlicing = true;
                break;
            }
            ++lookahead;
        }

        pos = savedPos; // rewind to '['
        if (isSlicing)
        {
            call = parseArraySlicingExpr(std::move(call));
        }
        else
        {
            call = parseArrayIndexingExpr(std::move(call));
        }
    }

    return call;
}

// ReSharper disable once CppDFAUnreachableFunctionCall
// called in parseArrayAccess when isSlicing = true
std::unique_ptr<Call> Parser::parseArraySlicingExpr(std::unique_ptr<Call> call)
{
    const Token& t = current();
    // Default start, stop, step
    std::unique_ptr<Expr> start = std::make_unique<ConstValueExpr>(
        t,
        symTable.getCurrScope(),
        symTable.getCurrFunc(),
        std::make_unique<Type>(L"degree"),
        L"0"
    );
    std::unique_ptr<Expr> stop = std::make_unique<ConstValueExpr>(
        t,
        symTable.getCurrScope(),
        symTable.getCurrFunc(),
        std::make_unique<Type>(L"degree"),
        L"-1"
    );
    std::unique_ptr<Expr> step = std::make_unique<ConstValueExpr>(
        t,
        symTable.getCurrScope(),
        symTable.getCurrFunc(),
        std::make_unique<Type>(L"degree"),
        L"1"
    );

    expect(Token::PUNCTUATION, L"[", MissingBrace(current()));

    if (!match(Token::PUNCTUATION, L":"))
    {
        start = parseExpr();
    }

    if (match(Token::PUNCTUATION, L":"))
    {
        advance(); // consume ':'

        if (!match(Token::PUNCTUATION, L":") && !match(Token::PUNCTUATION, L"]"))
        {
            stop = parseExpr();
        }


        if (match(Token::PUNCTUATION, L":"))
        {
            advance(); // consume ':'

            if (!match(Token::PUNCTUATION, L"]"))
            {
                step = parseExpr();
            }
        }
    }

    expect(Token::PUNCTUATION, L"]", MissingBrace(current()));

    return std::make_unique<ArraySlicingExpr>(
        t,
        symTable.getCurrScope(),
        symTable.getCurrFunc(),
        std::move(call),
        std::move(start),
        std::move(stop),
        std::move(step)
    );
}

std::unique_ptr<Call> Parser::parseArrayIndexingExpr(std::unique_ptr<Call> call)
{
    const Token& t = current();
    expect(Token::PUNCTUATION, L"[", MissingBrace(current()));
    std::unique_ptr<Expr> index = parseExpr();
    expect(Token::PUNCTUATION, L"]", MissingBrace(current()));

    return std::make_unique<ArrayIndexingExpr>(
        t,
        symTable.getCurrScope(),
        symTable.getCurrFunc(),
        std::move(call),
        std::move(index)
    );
}

std::unique_ptr<IType> Parser::parseIType()
{
    if (match(Token::TYPE, L"riff"))
    {
        advance();
        return parseArrayType();
    }

    return parseType();
}

std::unique_ptr<Type> Parser::parseType()
{
    std::wstring value = expectAndGet(Token::TYPE, InvalidNumberLiteral(current())).value;

    return std::make_unique<Type>(value);
}

std::unique_ptr<ArrayType> Parser::parseArrayType()
{
    std::unique_ptr<IType> arrType = parseIType();

    return std::make_unique<ArrayType>(std::move(arrType));
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
    const Token& t = current();
    // Handle "unary" operators: +, -, *, /, //
    if (t.value == L"+" || t.value == L"-" ||
         t.value == L"*" || t.value == L"/" ||
         t.value == L"//")
    {
        std::wstring op = t.value;
        advance(); // consume unary operator

        auto right = parseExpr();

        return std::make_unique<BinaryOpExpr>(
            t,
            symTable.getCurrScope(),
            symTable.getCurrFunc(),
            op,
            nullptr,          // left = nullptr signals unary
            std::move(right)
        );
    }

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
        return parseCallExpr();
    }


    if (match(Token::PUNCTUATION, L"("))
    {
        advance(); // consume '('
        auto expr = parseExpr(true);
        expect(Token::PUNCTUATION, L")", MissingParenthesis(current()));
        return expr;
    }

    throw UnexpectedToken(current());
}

std::unique_ptr<Expr> Parser::parseBinaryOpRight(int exprPrec, std::unique_ptr<Expr> left)
{

    const Token& t = current();
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

        left = std::make_unique<BinaryOpExpr>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), op, std::move(left), std::move(right));
    }
}

std::unique_ptr<ConstValueExpr> Parser::parseConstValueExpr()
{
    const Token& t = current();
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
    return std::make_unique<ConstValueExpr>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::make_unique<Type>(type), t.value);
}

std::unique_ptr<UnaryOpExpr> Parser::parseUnaryOpExpr(const bool isStmt)
{
    const Token& t = current();
    std::unique_ptr<Call> call = parseCallExpr();
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

    return std::make_unique<UnaryOpExpr>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(call), op, isStmt);
}
