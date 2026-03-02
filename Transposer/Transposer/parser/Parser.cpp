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
#include "../errorHandling/syntaxErrors/IllegalVarName.h"

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

// ---------- lexical errors ----------
#include "../errorHandling/lexicalErrors/InvalidNumberLiteral.h"
#include "../errorHandling/lexicalErrors/UnexpectedEOF.h"
#include "../errorHandling/lexicalErrors/UnrecognizedToken.h"

// ---------- class errors ----------
#include "../errorHandling/classErrors/ClassDosentExisit.h"
#include "../errorHandling/classErrors/IllegalFieldName.h"
#include "../errorHandling/classErrors/InvalidAccessKeyword.h"
#include "../errorHandling/classErrors/MissingClassPipe.h"
#include "../errorHandling/classErrors/NoCtor.h"
#include "../errorHandling/classErrors/RedefOfCtor.h"

// ---------- just how ----------
#include "../errorHandling/how/HowDidYouGetHere.h"

#include "files/FileGraph.h"
#include "symbols/Type/ClassType.h"


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
            Token pathToken;
            if (!expectAndGet(Token::CONST_STR, new ExpectedAPath(current()), pathToken)) return {};
            std::wstring wstr = pathToken.value;
            wstr.erase(
                std::ranges::remove(wstr, L'"').begin(),
                wstr.end()
            );
            std::filesystem::path path = wstr;
            if (!expect(Token::PUNCTUATION, L"║", new MissingSemicolon(current()))) return {};

            if (path.extension() != ".cb")
            {
                addError(new InvalidPathExtension(current()));
                return {};
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
    if (!block) return;
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
            addError(new IllegalCredit(creditsQ.front().getToken(), Utils::wstrToStr(creditsQ.front().getName())));
        }

        creditsQ.pop();
    }

    while (!callsQ.empty())
    {
        FuncCallExpr* call = callsQ.front();
        std::unique_ptr<IType> t = symTable.getCallType(call);

        if (t == nullptr)
        {
            addError(new IllegalCall(call->getToken(), Utils::wstrToStr(call->getName())));
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
    else oss << "#include \"" << hPath.generic_string() << "\"" << std::endl;

    for (const auto& stmt : stmts)
    {
        oss << std::endl << stmt->translateToCpp();
    }

    if (isMain)
    {
        oss << "int main()" << std::endl <<
            "{" << std::endl <<
            "\treturn prelude().getValue();" << std::endl <<
            "}" << std::endl;
    }

    return oss.str();
}

std::string Parser::translateToH(const bool isMain) const
{
    std::ostringstream oss;
    if (!isMain) oss << "#pragma once" << std::endl;

    oss << "#include <string>" << std::endl;
    oss << Utils::getAllObjIncludes() << std::endl;

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

const std::vector<std::unique_ptr<Error>>& Parser::getErrors() const
{
    return errors;
}

void Parser::addError(Error* err)
{
    errors.emplace_back(err);
}

void Parser::synchronize()
{
    advance();

    // Clear callsQ and creditsQ to prevent dangling pointers from incomplete statements/expressions
    while (!callsQ.empty()) callsQ.pop();
    while (!creditsQ.empty()) creditsQ.pop();

    while (!isAtEnd())
    {
        if (prev().type == Token::PUNCTUATION && prev().value == L"║") return;

        switch (current().type)
        {
            case Token::KEYWORD:
                // Check for keywords that start a new statement
                if (current().value == L"song" || current().value == L"instrument" ||
                    current().value == L"D" || current().value == L"G" ||
                    current().value == L"A" || current().value == L"Fmaj" ||
                    current().value == L"Fmin" || current().value == L"B" ||
                    current().value == L"feat" || current().value == L"play" ||
                    current().value == L"playBar" || current().value == L"hear" ||
                    current().value == L"pause" || current().value == L"resume")
                {
                    return;
                }
            default:
                break;
        }

        advance();
    }
}


const Token& Parser::current() const
{
    if (pos >= len) return tokens.back(); // Should not happen if isAtEnd is used properly
    return tokens[pos];
}

Token Parser::advance()
{
    if (!isAtEnd())
    {
        pos++;
    }
    return prev();
}

const Token& Parser::peek() const
{
    if (pos + 1 >= len)
    {
        return tokens.back();
    }
    return tokens[pos + 1];
}

const Token& Parser::prev() const
{
    if (pos == 0) return tokens.front();
    return tokens[pos - 1];
}

bool Parser::isAtEnd() const
{
    return pos >= len;
}

bool Parser::match(const Token::TokenType type) const
{
    if (isAtEnd()) return false;
    return current().type == type;
}

bool Parser::match(const Token::TokenType type, const std::wstring& value) const
{
    if (isAtEnd()) return false;
    const Token t = current();

    return t.type == type && t.value == value;
}

bool Parser::expect(const Token::TokenType type)
{
    if (!match(type))
    {
        addError(new UnexpectedToken(current()));
        return false;
    }
    advance();
    return true;
}

bool Parser::expect(const Token::TokenType type, const std::wstring& value)
{
    if (!match(type, value))
    {
        addError(new UnexpectedToken(current()));
        return false;
    }
    advance();
    return true;
}

bool Parser::expect(const Token::TokenType type, Error* err)
{
    if (!match(type))
    {
        addError(err);
        return false;
    }
    advance();
    delete err;
    return true;
}

bool Parser::expect(const Token::TokenType type, const std::wstring& value, Error* err)
{
    if (!match(type, value))
    {
        addError(err);
        return false;
    }
    advance();
    delete err;
    return true;
}

bool Parser::expectAndGet(Token::TokenType type, Token& out)
{
    if (!expect(type)) return false;
    out = prev();
    return true;
}

bool Parser::expectAndGet(Token::TokenType type, const std::wstring& value, Token& out)
{
    if (!expect(type, value)) return false;
    out = prev();
    return true;
}

bool Parser::expectAndGet(Token::TokenType type, Error* err, Token& out)
{
    if (!expect(type, err)) return false;
    out = prev();
    return true;
}

bool Parser::expectAndGet(Token::TokenType type, const std::wstring& value, Error* err, Token& out)
{
    if (!expect(type, value, err)) return false;
    out = prev();
    return true;
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
    if (!varType) return nullptr;

    Token identifierToken;
    if (!expectAndGet(Token::IDENTIFIER, new MissingIdentifier(current()), identifierToken)) return nullptr;
    const std::wstring varName = identifierToken.value;

    const Var var(varType->copy(), varName);
    
    const bool startsMusical = Utils::startsWithNote(varName);

    if (isField && !startsMusical)
    {
        addError(new IllegalFieldName(identifierToken));
        return nullptr;
    }

    if (!isField && startsMusical)
    {
        addError(new IllegalVarName(identifierToken));
        return nullptr;
    }

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

    if (!expect(Token::OP_ASSIGNMENT, L"=", new NoPlacementOperator(current()))) return nullptr;

    auto expr = parseExpr();
    if (!expr) return nullptr;

    if (!isField)
    {
        if (!expect(Token::PUNCTUATION, L"║", new MissingSemicolon(current()))) return nullptr;
        symTable.addVar(varType->copy(), identifierToken); 
    }

    return std::make_unique<VarDeclStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), true, std::move(expr), var);
}

std::unique_ptr<AssignmentStmt> Parser::parseAssignmentStmt()
{
    const Token& t = current();
    std::unique_ptr<Call> call = parseCallExpr();
    if (!call) return nullptr;

    Token opToken;
    if (!expectAndGet(Token::OP_ASSIGNMENT, opToken)) return nullptr;
    const std::wstring op = opToken.value;

    auto expr = parseExpr();
    if (!expr) return nullptr;
    if (!expect(Token::PUNCTUATION, L"║", new MissingSemicolon(current()))) return nullptr;

    return std::make_unique<AssignmentStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(call), op, std::move(expr));
}

std::unique_ptr<HearStmt> Parser::parseHearStmt()
{
    std::vector<std::unique_ptr<Call>> calls;
    const Token& t = current();
    if (!expect(Token::KEYWORD, L"hear", new HowDidYouGetHere(current()))) return nullptr;
    if (!expect(Token::PUNCTUATION, L"(", new MissingBrace(current()))) return nullptr;

    while (!match(Token::PUNCTUATION, L")"))
    {
        auto call = parseCallExpr();
        if (!call) return nullptr;
        calls.push_back(std::move(call));

        if (match(Token::PUNCTUATION, L")"))
        {
            break;
        }

        if (!expect(Token::PUNCTUATION, L",", new InvalidExpression(current()))) return nullptr;
    }
    advance();

    if (!expect(Token::PUNCTUATION, L"║", new MissingSemicolon(current()))) return nullptr;

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
        addError(new HowDidYouGetHere(current()));
        return nullptr;
    }

    if (!expect(Token::PUNCTUATION, L"(", new MissingBrace(current()))) return nullptr;

    while (!match(Token::PUNCTUATION, L")"))
    {
        auto arg = parseExpr();
        if (!arg) return nullptr;
        args.push_back(std::move(arg));

        if (match(Token::PUNCTUATION, L")"))
        {
            break;
        }

        if (!expect(Token::PUNCTUATION, L",", new InvalidExpression(prev()))) return nullptr;
    }
    advance(); 

    if (!expect(Token::PUNCTUATION, L"║", new MissingSemicolon(current()))) return nullptr;

    return std::make_unique<PlayStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(args), newline);
}

std::unique_ptr<BodyStmt> Parser::parseBodyStmt(const std::vector<std::pair<Var, const Token>>& args, const bool isGlobal, const bool isBreakable, const bool isContinueAble, const bool hasBrace)
{
    const Token& t = current();
    if (!isGlobal && hasBrace)
    {
        if (!expect(Token::PUNCTUATION, L"∮", new MissingBrace(current()))) return nullptr;
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

    while (!isAtEnd() && !match(Token::PUNCTUATION, L"☉")) 
    {
        std::unique_ptr<Stmt> stmt = nullptr;
        if (match(Token::TYPE))
        {
            stmt = parseVarDecStmt();
        }
        else if (match(Token::IDENTIFIER) && SymbolTable::isClass(current().value))
        {
            stmt = parseObjCreationStmt();
            if (!stmt)
            {
                synchronize();
                continue;
            }
        }
        else if (match(Token::IDENTIFIER) && isAssignmentStmtAhead())
        {
            stmt = parseAssignmentStmt();
        }
        else if (match(Token::KEYWORD, L"hear"))
        {
            stmt = parseHearStmt();
        }
        else if (match(Token::KEYWORD, L"play") ||
                 match(Token::KEYWORD, L"playBar"))
        {
            stmt = parsePlayStmt();
        }
        else if (isUnaryOpStmtAhead())
        {
            stmt = parseUnaryOpExpr(true);
        }
        else if (match(Token::KEYWORD, L"song"))
        {
            stmt = parseFuncDeclStmt();
        }
        else if (match(Token::KEYWORD, L"B"))
        {
            stmt = parseReturnStmt();
        }
        else if (match(Token::IDENTIFIER) && peek().type == Token::PUNCTUATION && peek().value == L"(")
        {
            stmt = parseFuncCallExpr(true);
        }
        else if (match(Token::IDENTIFIER_CALL))
        {
            stmt = parseConstractorCallStmt();
            if (stmt && !expect(Token::PUNCTUATION, L"║", new MissingSemicolon(current())))
            {
                stmt = nullptr;
            }
        }
        else if (match(Token::KEYWORD, L"D"))
        {
            stmt = parseIfStmt();
        }
        else if (match(Token::KEYWORD, L"G"))
        {
            stmt = (parseWhileStmt());
        }
        else if (match(Token::KEYWORD, L"A"))
        {
            stmt = parseSwitchStmt();
        }
        else if (match(Token::KEYWORD, L"pause"))
        {
            if (!symTable.getCurrScope()->getIsBreakable())
            {
                addError(new StmtNotBreakable(current()));
                synchronize();
                continue;
            }
            stmt = parseBreakStmt();
            if (!hasBrace)
            {
                break;
            }
        }
        else if (match(Token::KEYWORD, L"resume"))
        {
            if (!symTable.getCurrScope()->getIsContinueAble())
            {
                addError(new StmtNotContinueAble(current()));
                synchronize();
                continue;
            }
            stmt = parseContinueStmt();
            if (!hasBrace)
            {
                break;
            }
        }
        else if (match(Token::KEYWORD, L"Fmaj") || match(Token::KEYWORD, L"Fmin"))
        {
            stmt = parseForStmt();
        }
        else if (match(Token::KEYWORD, L"instrument"))
        {
            stmt = parseClassDeclStmt();
        }
        else if (match(Token::COMMENT_MULTI) || match(Token::COMMENT_SINGLE))
        {
            advance();
            continue;
        }
        else if (match(Token::IDENTIFIER) && peek().type == Token::PUNCTUATION && peek().value == L"\\")
        {
            stmt = parseExpr(false, true);
        }
        else if (match(Token::KEYWORD, L"feat"))
        {
            addError(new IncludeNotInTop(current()));
            synchronize();
            continue;
        }
        else
        {
            addError(new UnrecognizedToken(current()));
            synchronize();
            continue;
        }

        if(stmt)
        {
            bodyStmts.push_back(std::move(stmt));
        }
        else
        {
            synchronize();
        }
    }

    if (!isGlobal && hasBrace)
    {
        if (isAtEnd())
        {
            addError(new MissingBrace(prev()));
        }

        if (!match(Token::PUNCTUATION, L"☉"))
        {
            addError(new MissingBrace(current()));
        } else if (!isAtEnd())
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

    if (!expect(Token::KEYWORD, L"song", new HowDidYouGetHere(current()))) return nullptr;

    if (match(Token::PUNCTUATION, L"©"))
    {
        advance();
        if (!expect(Token::PUNCTUATION, L"(", new MissingBrace(current()))) return nullptr;
        while (!match(Token::PUNCTUATION, L")"))
        {
            auto credit = parseFuncCreditStmt();
            if (!credit) return nullptr;
            credited.push_back(std::move(credit));

            if (!match(Token::PUNCTUATION, L")"))
            {
                if (!expect(Token::PUNCTUATION, L",", new UnexpectedToken(current()))) return nullptr;
            }
        }
        advance();
    }

    Token funcNameToken;
    if (!expectAndGet(Token::IDENTIFIER, new MissingIdentifier(current()), funcNameToken)) return nullptr;
    const std::wstring funcName = funcNameToken.value;


    if (funcName == L"prelude" && hasMain)
    {
        addError(new MainOverride(current()));
    }

    if (symTable.doesFuncExist(funcName))
    {
        addError(new IdentifierTaken(current()));
    }

    if (!expect(Token::PUNCTUATION, L"(", new MissingParenthesis(current()))) return nullptr;
    while (!match(Token::PUNCTUATION, L")"))
    {
        const std::unique_ptr<IType> type = parseIType();

        if (!type) return nullptr;

        Token currNameToken;
        if (!expectAndGet(Token::IDENTIFIER, new MissingBrace(current()), currNameToken)) return nullptr;
        std::wstring currName = currNameToken.value;

        args.emplace_back(Var(type->copy(), currName), current());

        if (!match(Token::PUNCTUATION, L")"))
        {
            if (!expect(Token::PUNCTUATION, L",", new UnexpectedToken(current()))) return nullptr;
        }
    }
    advance();

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
            addError(new InvalidMainReturnType(current()));
        }

        auto funcDeclStmt = std::make_unique<FuncDeclStmt>(t, symTable.getCurrScope(), symTable.getCurrClass(), funcName, std::make_unique<Type>(L"fermata"), varArgs, credited);
        symTable.addFunc(funcDeclStmt->getFunc());
        symTable.changeFunc(funcDeclStmt.get());
        auto body = parseBodyStmt(args);
        if (!body) return nullptr;
        funcDeclStmt->setBody(std::move(body));
        symTable.changeFunc(currFunc);
        return std::move(funcDeclStmt);
    }
    advance(); 

    const std::unique_ptr<IType> rType = parseIType();
    if (!rType) return nullptr;

    if (funcName == L"prelude")
    {
        if (!args.empty())
        {
            addError(new InvalidMainArgs(current()));
        }

        if (rType->getType() != L"degree")
        {
            addError(new InvalidMainReturnType(current()));
        }

        hasMain = true;
    }

    auto funcDeclStmt = std::make_unique<FuncDeclStmt>(t, symTable.getCurrScope(), symTable.getCurrClass(), funcName, rType->copy(), varArgs, credited);

    if (!isMethod) symTable.addFunc(funcDeclStmt->getFunc());
    symTable.changeFunc(funcDeclStmt.get());

    auto body = parseBodyStmt(args);
    if (!body) return nullptr;
    funcDeclStmt->setBody(std::move(body));

    if (!funcDeclStmt->getHasReturned())
    {
        addError(new NoReturn(prev()));
    }

    symTable.changeFunc(currFunc);
    return std::move(funcDeclStmt);
}

std::unique_ptr<ReturnStmt> Parser::parseReturnStmt()
{
    const Token& t = current();
    if (!expect(Token::KEYWORD, L"B", new HowDidYouGetHere(current()))) return nullptr;

    IFuncDeclStmt* currFunc = symTable.getCurrFunc();
    std::unique_ptr<Expr> expr = nullptr;

    if (currFunc->getReturnType()->getType() != L"fermata")
    {
        if (!expect(Token::PUNCTUATION, L"\\", new ExpectedAnExpression(current()))) return nullptr;
        expr = parseExpr();
        if (!expr) return nullptr;
        if (*(currFunc->getReturnType()) != *(expr->getType()))
        {
            addError(new WrongReturnType(current()));
        }
    }

    if (!expect(Token::PUNCTUATION, L"║", new MissingSemicolon(current()))) return nullptr;

    currFunc->setHasReturned(true);

    return std::make_unique<ReturnStmt>(t, symTable.getCurrScope(), currFunc, symTable.getCurrClass(), expr);
}

std::unique_ptr<FuncCreditStmt> Parser::parseFuncCreditStmt()
{
    const Token& t = current();
    Token creditToken;
    if (!expectAndGet(Token::IDENTIFIER, new MissingIdentifier(current()), creditToken)) return nullptr;
    
    FuncCredit credit(creditToken.value, current());

    creditsQ.push(credit);

    return std::make_unique<FuncCreditStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), credit);
}

std::unique_ptr<IfStmt> Parser::parseIfStmt()
{
    const Token& t = current();
    if (!expect(Token::KEYWORD, L"D", new HowDidYouGetHere(current()))) return nullptr;
    if (!expect(Token::PUNCTUATION, L"|", new MissingPipe(current()))) return nullptr;
    std::unique_ptr<Expr> expr = parseExpr();
    if (!expr) return nullptr;
    if (!expect(Token::PUNCTUATION, L"|", new MissingPipe(current()))) return nullptr;
    constexpr std::vector<std::pair<Var, const Token>> args;
    std::unique_ptr<Stmt> body = parseBodyStmt(args, false);
    if (!body) return nullptr;

    if (match(Token::KEYWORD, L"E"))
    {
        advance();
        if (match(Token::PUNCTUATION, L"\\"))
        {
            advance();
            auto elseIf = parseIfStmt();
            if (!elseIf) return nullptr;
            return std::make_unique<IfStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(expr), std::move(body), std::move(elseIf), true);
        }
        auto elseBody = parseBodyStmt(args, false);
        if (!elseBody) return nullptr;
        return std::make_unique<IfStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(expr), std::move(body), std::move(elseBody), false);
    }
    return std::make_unique<IfStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(expr), std::move(body), nullptr, false);
}

std::unique_ptr<ArrayDeclStmt> Parser::parseArrayDeclStmt()
{
    const Token& t = current();
    std::unique_ptr<IType> arrType = parseIType();
    if (!arrType) return nullptr;

    const unsigned int arrLevel = arrType->getArrLevel();
    Token nameToken;
    if (!expectAndGet(Token::IDENTIFIER, new MissingIdentifier(current()), nameToken)) return nullptr;
    const std::wstring name = nameToken.value;

    std::vector<std::unique_ptr<Expr>> sizes;
    std::unique_ptr<Expr> startExpr = nullptr;

    Var var(std::move(arrType), name);

    symTable.addVar(var.copy(), current());

    for (unsigned int i = 0; i < arrLevel; i++)
    {
        if (!expect(Token::PUNCTUATION, L"[", new MissingBrace(current()))) return nullptr;

        if (match(Token::PUNCTUATION, L"]"))
        {
            advance();
            sizes.push_back(
                std::make_unique<ConstValueExpr>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::make_unique<Type>(L"degree"), L"1")
                );
        }
        else
        {
            auto sizeExpr = parseExpr();
            if (!sizeExpr) return nullptr;
            sizes.push_back(std::move(sizeExpr));
            if (!expect(Token::PUNCTUATION, L"]", new MissingBrace(current()))) return nullptr;
        }
    }

    if (match(Token::OP_ASSIGNMENT, L"="))
    {
        advance();
        startExpr = parseExpr();
        if (!startExpr) return nullptr;
    }

    if (!expect(Token::PUNCTUATION, L"║", new MissingSemicolon(current()))) return nullptr;
    return std::make_unique<ArrayDeclStmt>(
        t,
        symTable.getCurrScope(),
        symTable.getCurrFunc(),
        symTable.getCurrClass(),
        startExpr != nullptr,
        std::move(startExpr),
        var,
        std::move(sizes)
    );
}

std::unique_ptr<WhileStmt> Parser::parseWhileStmt()
{
    const Token& t = current();
    if (!expect(Token::KEYWORD, L"G", new HowDidYouGetHere(current()))) return nullptr;
    if (!expect(Token::PUNCTUATION, L"║:", new MissingBrace(current()))) return nullptr;
    std::unique_ptr<Expr> expr = parseExpr();
    if (!expr) return nullptr;
    if (!expect(Token::PUNCTUATION, L":║", new MissingBrace(current()))) return nullptr;
    constexpr std::vector<std::pair<Var, const Token>> args; 
    std::unique_ptr<Stmt> body = parseBodyStmt(args, false, true, true, true);
    if (!body) return nullptr;

    return std::make_unique<WhileStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), expr, body);
}

std::unique_ptr<BreakStmt> Parser::parseBreakStmt()
{
    const Token& t = current();
    if (!expect(Token::KEYWORD, L"pause", new HowDidYouGetHere(current()))) return nullptr;
    if (!expect(Token::PUNCTUATION, L"║", new MissingSemicolon(current()))) return nullptr;

    return std::make_unique<BreakStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass());
}

std::unique_ptr<CaseStmt> Parser::parseCaseStmt()
{
    bool isDefault = false;
    const Token& t = current();

    if (!expect(Token::KEYWORD, L"C")) return nullptr;
    std::unique_ptr<Expr> e;
    if (match(Token::PUNCTUATION, L"\\"))
    {
        advance();
        e = parseExpr();
        if (!e) return nullptr;
    }
    else
    {
        isDefault = true;
    }
    if (!expect(Token::PUNCTUATION, L"|", new MissingPipe(current()))) return nullptr;

    symTable.enterScope(true, false);
    std::vector<std::pair<Var, const Token>> args;
    std::unique_ptr<BodyStmt> body = parseBodyStmt(args, false, true, false, false);
    if (!body) return nullptr;
    body->setHasBrace(false);

    symTable.exitScope();

    return std::make_unique<CaseStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(e), std::move(body), isDefault);
}

std::unique_ptr<SwitchStmt> Parser::parseSwitchStmt()
{
    const Token& t = current();
    if (!expect(Token::KEYWORD, L"A", new HowDidYouGetHere(current()))) return nullptr;
    if (!expect(Token::PUNCTUATION, L"\\")) return nullptr;
    const std::optional<Var> v = symTable.getCurrScope()->getVar(current().value, symTable.getCurrClass())->copy();

    if (v == std::nullopt)
    {
        addError(new UnrecognizedIdentifier(current()));
        return nullptr;
    }

    advance();
    if (!expect(Token::PUNCTUATION, L"∮", new MissingBrace(current()))) return nullptr;
    std::vector<std::unique_ptr<CaseStmt>> cases;

    while (!match(Token::PUNCTUATION, L"☉"))
    {
        if (match(Token::KEYWORD, L"C"))
        {
            auto caseStmt = parseCaseStmt();
            if (!caseStmt) {
                synchronize();
                continue;
            }
            cases.push_back(std::move(caseStmt));
        }
        else
        {
            addError(new StmtInsideSwitchThatIsNotCase(current()));
            synchronize();
        }
    }

    if (!expect(Token::PUNCTUATION, L"☉", new MissingBrace(current()))) return nullptr;

    return std::make_unique<SwitchStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), v.value().copy(), cases);
}

std::unique_ptr<ContinueStmt> Parser::parseContinueStmt()
{
    const Token& t = current();
    if (!expect(Token::KEYWORD, L"resume", new MissingBrace(current()))) return nullptr;
    if (!expect(Token::PUNCTUATION, L"║", new MissingSemicolon(current()))) return nullptr;

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
    Token varToken;

    if (match(Token::KEYWORD, L"Fmaj"))
    {
        isIncreasing = true;
        advance();
    }
    else
    {
        if (!expect(Token::KEYWORD, L"Fmin", new HowDidYouGetHere(current()))) return nullptr;
    }

    startExpr = parseExpr(false, false, false);
    if (!startExpr) return nullptr;


    if ((isIncreasing && match(Token::OP_UNARY, L"♯"))|| (!isIncreasing && match(Token::OP_UNARY, L"♭")))
    {
        advance();
        stepExpr = parseExpr(false, false, false);
        if (!stepExpr) return nullptr;
    }

    if (match(Token::PUNCTUATION, L"#"))
    {
        advance();
        stopExpr = parseExpr(false, false, false);
        if (!stopExpr) return nullptr;
    }

    if (match(Token::PUNCTUATION, L"\\"))
    {
        advance();
        if (!expectAndGet(Token::IDENTIFIER, new MissingIdentifier(current()), varToken)) return nullptr;
        varName = varToken.value;
    } else {
        varToken = Token(Token::IDENTIFIER, L"i", current().line, current().column, current().path);
    }

    std::vector<std::pair<Var, const Token>> args;
    args.emplace_back( Var(std::make_unique<Type>(L"degree"), varName), varToken);
    body = parseBodyStmt(args, false, true, true, true);
    if (!body) return nullptr;

    return std::make_unique<ForStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(),
        std::move(body), isIncreasing, std::move(startExpr), std::move(stepExpr), std::move(stopExpr), varName);
}

std::unique_ptr<ClassDeclStmt> Parser::parseClassDeclStmt()
{
    bool isInheriting = false;
    std::wstring type;
    std::wstring inheritingName = L"";

    Token t = current();
    if (!expect(Token::KEYWORD, L"instrument", new HowDidYouGetHere(t))) return nullptr;
    
    Token nameToken;
    if (!expectAndGet(Token::IDENTIFIER, new MissingIdentifier(current()), nameToken)) return nullptr;
    const std::wstring name = nameToken.value;

    if (match(Token::PUNCTUATION, L"|"))
    {
        advance();
        isInheriting = true;
        if (match(Token::KEYWORD, L"tutti"))
        {
            type = current().value;
        }
        else if (match(Token::KEYWORD, L"sordino"))
        {
            type = current().value;
        }
        else if (match(Token::KEYWORD, L"section"))
        {
            type = current().value;
        }
        else
        {
            addError(new InvalidAccessKeyword(current()));
            return nullptr;
        }
        advance();
        if (!expectAndGet(Token::IDENTIFIER, new MissingIdentifier(current()), nameToken)) return nullptr;
        if (SymbolTable::getClass(nameToken.value) == nullptr)
        {
            addError(new UnrecognizedIdentifier(current()));
            return nullptr;
        }
        inheritingName = nameToken.value;
    }

    std::vector<Field> fields;
    std::vector<Method> methods;
    std::vector<Ctor> ctors;

    ClassNode* parentPtr = nullptr;

    if (!inheritingName.empty())
    {
        parentPtr = SymbolTable::getClass(inheritingName);
    }

    symTable.enterScope(false, false);
    symTable.addClass(Class(name), parentPtr);

    const size_t tempPos = pos;

    if (!expect(Token::PUNCTUATION, L"∮", new MissingBrace(current()))) return nullptr;
    if (!parseFields(fields)) return nullptr;
    if (!expect(Token::PUNCTUATION, L"☉", new MissingBrace(current()))) return nullptr;

    pos = tempPos;

    if (!expect(Token::PUNCTUATION, L"∮", new MissingBrace(current()))) return nullptr;
    if (!parseMethods(methods)) return nullptr;
    if (!expect(Token::PUNCTUATION, L"☉", new MissingBrace(current()))) return nullptr;

    pos = tempPos;

    if (!expect(Token::PUNCTUATION, L"∮", new MissingBrace(current()))) return nullptr;
    if (!parseCtors(ctors)) return nullptr;
    if (!expect(Token::PUNCTUATION, L"☉", new MissingBrace(current()))) return nullptr;

    auto declStmt = std::make_unique<ClassDeclStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), name, fields, methods, ctors, isInheriting, type, inheritingName);

    if (!declStmt->getHasEmptyCtor())
    {
        std::vector<Var> emptyArgs;
        symTable.addCtor(PUBLIC, Constractor(emptyArgs, name) ,t);
    }

    symTable.resetCurrClass();
    symTable.exitScope();
    return std::move(declStmt);
}

std::unique_ptr<ConstractorDeclStmt> Parser::parseCtor()
{
    const Token& t = current();
    std::vector<std::pair<Var, const Token>> args;
    std::vector<std::pair<Var, const Token>> emptyArgs;
    std::vector<std::unique_ptr<FuncCreditStmt>> credited;
    IFuncDeclStmt* currFunc = symTable.getCurrFunc();

    if (!expect(Token::IDENTIFIER_CALL, new MissingIdentifier(t))) return nullptr;

    const std::wstring funcName = t.value;

    const ClassNode* pClass = symTable.getCurrClass();

    if (pClass == nullptr) {
        addError(new HowDidYouGetHere(t));
        return nullptr;
    }

    // entering scope so we can do parent inits
    symTable.enterScope(false, false);

    if (!expect(Token::PUNCTUATION, L"(", new MissingParenthesis(current()))) return nullptr;
    while (!match(Token::PUNCTUATION, L")"))
    {
        const std::unique_ptr<IType> type = parseIType();

        if (!type) return nullptr;

        Token currNameToken;
        if (!expectAndGet(Token::IDENTIFIER, new MissingBrace(current()), currNameToken)) return nullptr;
        std::wstring currName = currNameToken.value;

        args.emplace_back(Var(type->copy(), currName), current());

        if (!match(Token::PUNCTUATION, L")"))
        {
            if (!expect(Token::PUNCTUATION, L",", new UnexpectedToken(current()))) return nullptr;
        }
    }
    advance();

    std::vector<Var> varArgs;

    if (!args.empty())
    {
        for (const auto& [var, varToken] : args )
        {
            varArgs.emplace_back(var.copy());
            symTable.addVar(var.copy(), varToken);
        }
    }

    if (pClass->getClass().hasConstractor(Constractor(varArgs, funcName)))
    {
        addError(new RedefOfCtor(t));
    }

    auto ctorDeclStmt = std::make_unique<ConstractorDeclStmt>(t, symTable.getCurrScope(), pClass, pClass->getClass().getClassName(), varArgs);
    symTable.changeFunc(ctorDeclStmt.get());

    if (match(Token::PUNCTUATION, L"\\"))
    {
        advance();
        Token token = current();
        if (!expectAndGet(Token::KEYWORD, L"bass", token) || !expectAndGet(Token::PUNCTUATION, L"(", new MissingBrace(token), token)) return nullptr;

        std::vector<std::unique_ptr<Expr>> parentArgs;

        while (!match(Token::PUNCTUATION, L")"))
        {
            auto expr = parseExpr();

            if (expr == nullptr) return nullptr;

            parentArgs.emplace_back(std::move(expr));

            if (!match(Token::PUNCTUATION, L")"))
            {
                if (!expect(Token::PUNCTUATION, L",", new UnexpectedToken(current()))) return nullptr;
            }
        }

        token = current();
        if (!expectAndGet(Token::PUNCTUATION, L")", new MissingBrace(current()), token)) return nullptr;
        ctorDeclStmt->setParentCtorCall(std::move(parentArgs));
    }

    auto body = parseBodyStmt(emptyArgs); // we already added the args
    if (!body) return nullptr;
    ctorDeclStmt->setBody(std::move(body));

    symTable.exitScope();
    symTable.changeFunc(currFunc);
    return std::move(ctorDeclStmt);
}

std::unique_ptr<ConstractorCallStmt> Parser::parseConstractorCallStmt()
{
    const Token& t = current();
    std::vector<std::unique_ptr<Expr>> args;

    Token callToken;
    if (!expectAndGet(Token::IDENTIFIER_CALL, new NoCtor(current()), callToken)) return nullptr;
    const std::wstring call = callToken.value;
    const std::wstring className = call.substr(0, call.length() - 5); 
    auto c = SymbolTable::getClass(className);

    if (c == nullptr) {
        addError(new ClassDosentExisit(t, Utils::wstrToStr(className)));
        return nullptr;
    }

    if (!expect(Token::PUNCTUATION, L"(", new MissingParenthesis(current()))) return nullptr;

    bool first = true;
    while (!match(Token::PUNCTUATION, L")"))
    {
        auto arg = parseExpr();
        if (!arg) return nullptr;
        args.push_back(std::move(arg));

        if (!first)
        {
            if (!expect(Token::PUNCTUATION, L",")) return nullptr;
        }
        first = false;
    }
    advance();
    return std::make_unique<ConstractorCallStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), c, std::move(args));
}

std::unique_ptr<ObjCreationStmt> Parser::parseObjCreationStmt()
{
    const Token& t = current();
    Token classNameToken;
    if (!expectAndGet(Token::IDENTIFIER, new MissingIdentifier(t), classNameToken)) return nullptr;
    const std::wstring className = classNameToken.value;

    auto c = SymbolTable::getClass(className);

    if (c == nullptr) {
        addError(new ClassDosentExisit(t, Utils::wstrToStr(className)));
        return nullptr;
    }
    
    Token nameToken;
    if (!expectAndGet(Token::IDENTIFIER, new MissingIdentifier(t), nameToken)) return nullptr;
    const std::wstring name = nameToken.value;

    const Var v = Var(std::make_unique<ClassType>(c),name);

    if (match(Token::OP_ASSIGNMENT, L"="))
    {
        advance();
        auto ctorCall = parseConstractorCallStmt();
        if (!ctorCall) return nullptr;
        auto res = std::make_unique<ObjCreationStmt>(
            t,
            symTable.getCurrScope(),
            symTable.getCurrFunc(),
            symTable.getCurrClass(),
            c,
            true,
            std::move(ctorCall),
            v.copy()
            );

        symTable.addVar(v.copy(), t);
        if (!expect(Token::PUNCTUATION, L"║", new MissingSemicolon(current()))) return nullptr;
        return std::move(res);
    }

    symTable.addVar(v.copy(), t);
    if (!expect(Token::PUNCTUATION, L"║", new MissingSemicolon(current()))) return nullptr;
    return std::make_unique<ObjCreationStmt>(
            t,
            symTable.getCurrScope(),
            symTable.getCurrFunc(),
            symTable.getCurrClass(),
            c,
            false,
            nullptr,
            v.copy()
        );
}

bool Parser::parseCtorCall(const ClassNode* c)
{
    const Token& t = current();
    std::vector<std::pair<Var, const Token>> args;
    
    Token funcNameToken;
    if (!expectAndGet(Token::IDENTIFIER_CALL, new MissingIdentifier(t), funcNameToken)) return false;
    const std::wstring funcName = funcNameToken.value;

    if (c == nullptr) {
        addError(new HowDidYouGetHere(t));
        return false;
    }

    if (!expect(Token::PUNCTUATION, L"(", new MissingParenthesis(current()))) return false;
    while (!match(Token::PUNCTUATION, L")"))
    {
        const std::unique_ptr<IType> type = parseIType();
        if (!type) return false;
        
        Token currNameToken;
        if (!expectAndGet(Token::IDENTIFIER, new MissingBrace(current()), currNameToken)) return false;
        std::wstring currName = currNameToken.value;


        args.emplace_back(Var(type->copy(), currName), current());

        if (!match(Token::PUNCTUATION, L")"))
        {
            if (!expect(Token::PUNCTUATION, L",", new UnexpectedToken(current()))) return false;
        }
    }
    advance(); 

    std::vector<Var> varArgs;

    if (!args.empty())
    {
        for (const auto& key : args | std::views::keys)
        {
            varArgs.emplace_back(key.copy());
        }
    }

    return c->getClass().hasConstractor(Constractor(varArgs, funcName));
}

bool Parser::parseFields(std::vector<Field>& fields)
{
    while (!isAtEnd() && !match(Token::PUNCTUATION, L"☉"))
    {
        if (match(Token::KEYWORD, L"playerScore"))
        {
            advance();
            if (!expect(Token::PUNCTUATION, L"∮", new MissingBrace(current()))) return false;

            while (true)
            {
                if (match(Token::TYPE) || SymbolTable::getClass(current().value))
                {
                    auto field = parseVarDecStmt(true);
                    if (!field) {
                        synchronize();
                        continue;
                    }

                    if (!symTable.addField(PRIVATE, field->getVar().copy(), current()))
                    {
                        addError(new HowDidYouGetHere(current()));
                    }

                    fields.emplace_back(PRIVATE, std::move(field));
                }
                else 
                {
                    unsigned int level = 0;
                    while (true)
                    {
                        if (isAtEnd() || (level == 0 && (match(Token::PUNCTUATION, L"|") || match(Token::PUNCTUATION, L"║")))) break;

                        if (match(Token::PUNCTUATION, L"∮")) level++;
                        else if (match(Token::PUNCTUATION, L"☉")) level--;
                        advance();
                    }
                }

                if (match(Token::PUNCTUATION, L"║"))
                {
                    break;
                }

                if (!expect(Token::PUNCTUATION, L"|", new MissingClassPipe(current()))) return false;
            }
            advance(); 
            if (!expect(Token::PUNCTUATION, L"☉", new MissingBrace(current()))) return false;
        }
        else if (match(Token::KEYWORD, L"conductorScore"))
        {
            advance();
            if (!expect(Token::PUNCTUATION, L"∮", new MissingBrace(current()))) return false;

            while (true)
            {
                if (match(Token::TYPE) || SymbolTable::getClass(current().value))
                {
                    auto field = parseVarDecStmt(true);
                     if (!field) {
                        synchronize();
                        continue;
                    }

                    if (!symTable.addField(PUBLIC, field->getVar().copy(), current()))
                    {
                        addError(new HowDidYouGetHere(current()));
                    }

                    fields.emplace_back(PUBLIC, std::move(field));
                }
                else 
                {
                    unsigned int level = 0;
                    while (true)
                    {
                        if (isAtEnd() || (level == 0 && (match(Token::PUNCTUATION, L"|") || match(Token::PUNCTUATION, L"║")))) break;

                        if (match(Token::PUNCTUATION, L"∮")) level++;
                        else if (match(Token::PUNCTUATION, L"☉")) level--;
                        advance();
                    }
                }

                if (match(Token::PUNCTUATION, L"║"))
                {
                    break;
                }

                if (!expect(Token::PUNCTUATION, L"|", new MissingClassPipe(current()))) return false;
            }
            advance();
            if (!expect(Token::PUNCTUATION, L"☉", new MissingBrace(current()))) return false;
        }
        else if (match(Token::KEYWORD, L"sectionScore"))
        {
            advance();
            if (!expect(Token::PUNCTUATION, L"∮", new MissingBrace(current()))) return false;

            while (true)
            {
                if (match(Token::TYPE) || SymbolTable::getClass(current().value))
                {
                    auto field = parseVarDecStmt(true);
                    if (!field) {
                        synchronize();
                        continue;
                    }

                    if (!symTable.addField(PROTECTED, field->getVar().copy(), current()))
                    {
                        addError(new HowDidYouGetHere(current()));
                    }

                    fields.emplace_back(PROTECTED, std::move(field));
                }
                else
                {
                    unsigned int level = 0;
                    while (true)
                    {
                        if (isAtEnd() || (level == 0 && (match(Token::PUNCTUATION, L"|") || match(Token::PUNCTUATION, L"║")))) break;

                        if (match(Token::PUNCTUATION, L"∮")) level++;
                        else if (match(Token::PUNCTUATION, L"☉")) level--;
                        advance();
                    }
                }

                if (match(Token::PUNCTUATION, L"║"))
                {
                    break;
                }

                if (!expect(Token::PUNCTUATION, L"|", new MissingClassPipe(current()))) return false;
            }
            advance();
            if (!expect(Token::PUNCTUATION, L"☉", new MissingBrace(current()))) return false;
        }
        else
        {
            addError(new UnrecognizedIdentifier(current()));
            synchronize();
        }
    }

    if (isAtEnd())
    {
        addError(new UnexpectedEOF(tokens[len-1]));
        return false;
    }
    return true;
}

bool Parser::parseMethods(std::vector<Method>& methods)
{
    while (!isAtEnd() && !match(Token::PUNCTUATION, L"☉"))
    {
        if (match(Token::KEYWORD, L"playerScore"))
        {
            advance();
            if (!expect(Token::PUNCTUATION, L"∮", new MissingBrace(current()))) return false;

            while (true)
            {
                if (match(Token::KEYWORD, L"song"))
                {
                    auto method = parseFuncDeclStmt();
                    if (!method) {
                        synchronize();
                        continue;
                    }
                    if (!symTable.addMethod(PRIVATE, method->getFunc().copy(), current()))
                    {
                        addError(new HowDidYouGetHere(current()));
                    }
                    methods.emplace_back(PRIVATE, std::move(method));
                }
                else
                {
                    unsigned int level = 0;
                    while (true)
                    {
                        if (isAtEnd() || (level == 0 && (match(Token::PUNCTUATION, L"|") || match(Token::PUNCTUATION, L"║")))) break;

                        if (match(Token::PUNCTUATION, L"∮")) level++;
                        else if (match(Token::PUNCTUATION, L"☉")) level--;
                        advance();
                    }
                }

                if (match(Token::PUNCTUATION, L"║"))
                {
                    break;
                }

                if (!expect(Token::PUNCTUATION, L"|", new MissingClassPipe(current()))) return false;
            }
            advance(); 
            if (!expect(Token::PUNCTUATION, L"☉", new MissingBrace(current()))) return false;
        }
        else if (match(Token::KEYWORD, L"conductorScore"))
        {
            advance();
            if (!expect(Token::PUNCTUATION, L"∮", new MissingBrace(current()))) return false;

            while (true)
            {
                if (match(Token::KEYWORD, L"song"))
                {
                    auto method = parseFuncDeclStmt();
                    if (!method) {
                        synchronize();
                        continue;
                    }
                    if (!symTable.addMethod(PUBLIC, method->getFunc().copy(), current()))
                    {
                        addError(new HowDidYouGetHere(current()));
                    }
                    methods.emplace_back(PUBLIC, std::move(method));
                }
                else
                {
                    unsigned int level = 0;
                    while (true)
                    {
                        if (isAtEnd() || (level == 0 && (match(Token::PUNCTUATION, L"|") || match(Token::PUNCTUATION, L"║")))) break;

                        if (match(Token::PUNCTUATION, L"∮")) level++;
                        else if (match(Token::PUNCTUATION, L"☉")) level--;
                        advance();
                    }
                }

                if (match(Token::PUNCTUATION, L"║"))
                {
                    break;
                }

                if (!expect(Token::PUNCTUATION, L"|", new MissingClassPipe(current()))) return false;
            }
            advance();
            if (!expect(Token::PUNCTUATION, L"☉", new MissingBrace(current()))) return false;
        }
        else if (match(Token::KEYWORD, L"sectionScore"))
        {
            advance();
            if (!expect(Token::PUNCTUATION, L"∮", new MissingBrace(current()))) return false;

            while (true)
            {
                if (match(Token::KEYWORD, L"song"))
                {
                    auto method = parseFuncDeclStmt();
                    if (!method) {
                        synchronize();
                        continue;
                    }
                    if (!symTable.addMethod(PROTECTED, method->getFunc().copy(), current()))
                    {
                        addError(new HowDidYouGetHere(current()));
                    }
                    methods.emplace_back(PROTECTED, std::move(method));
                }
                else
                {
                    unsigned int level = 0;
                    while (true)
                    {
                        if (isAtEnd() || (level == 0 && (match(Token::PUNCTUATION, L"|") || match(Token::PUNCTUATION, L"║")))) break;

                        if (match(Token::PUNCTUATION, L"∮")) level++;
                        else if (match(Token::PUNCTUATION, L"☉")) level--;
                        advance();
                    }
                }

                if (match(Token::PUNCTUATION, L"║"))
                {
                    break;
                }

                if (!expect(Token::PUNCTUATION, L"|", new MissingClassPipe(current()))) return false;
            }
            advance();
            if (!expect(Token::PUNCTUATION, L"☉", new MissingBrace(current()))) return false;
        }
        else
        {
            addError(new UnrecognizedIdentifier(current()));
            synchronize();
        }
    }

    if (isAtEnd())
    {
        addError(new UnexpectedEOF(tokens[len-1]));
        return false;
    }
    return true;
}

bool Parser::parseCtors(std::vector<Ctor>& ctors)
{
    while (!isAtEnd() && !match(Token::PUNCTUATION, L"☉"))
    {
        if (match(Token::KEYWORD, L"playerScore"))
        {
            advance();
            if (!expect(Token::PUNCTUATION, L"∮", new MissingBrace(current()))) return false;

            while (true)
            {
                if (match(Token::IDENTIFIER_CALL))
                {
                    auto ctor = parseCtor();
                    if (!ctor) {
                        synchronize();
                        continue;
                    }
                    if (!symTable.addCtor(PRIVATE, ctor->getConstractor().copy(), current()))
                    {
                        addError(new HowDidYouGetHere(current()));
                    }
                    ctors.emplace_back(PRIVATE, std::move(ctor));
                }
                else 
                {
                    unsigned int level = 0;
                    while (true)
                    {
                        if (isAtEnd() || (level == 0 && (match(Token::PUNCTUATION, L"|") || match(Token::PUNCTUATION, L"║")))) break;

                        if (match(Token::PUNCTUATION, L"∮")) level++;
                        else if (match(Token::PUNCTUATION, L"☉")) level--;
                        advance();
                    }
                }

                if (match(Token::PUNCTUATION, L"║"))
                {
                    break;
                }

                if (!expect(Token::PUNCTUATION, L"|", new MissingClassPipe(current()))) return false;
            }
            advance();
            if (!expect(Token::PUNCTUATION, L"☉", new MissingBrace(current()))) return false;
        }
        else if (match(Token::KEYWORD, L"conductorScore"))
        {
            advance();
            if (!expect(Token::PUNCTUATION, L"∮", new MissingBrace(current()))) return false;

            while (true)
            {
                if (match(Token::IDENTIFIER_CALL))
                {
                    auto ctor = parseCtor();
                    if (!ctor) {
                        synchronize();
                        continue;
                    }
                    if (!symTable.addCtor(PUBLIC, ctor->getConstractor().copy(), current()))
                    {
                        addError(new HowDidYouGetHere(current()));
                    }
                    ctors.emplace_back(PUBLIC, std::move(ctor));
                }
                else
                {
                    unsigned int level = 0;
                    while (true)
                    {
                        if (isAtEnd() || (level == 0 && (match(Token::PUNCTUATION, L"|") || match(Token::PUNCTUATION, L"║")))) break;

                        if (match(Token::PUNCTUATION, L"∮")) level++;
                        else if (match(Token::PUNCTUATION, L"☉")) level--;
                        advance();
                    }
                }

                if (match(Token::PUNCTUATION, L"║"))
                {
                    break;
                }

                if (!expect(Token::PUNCTUATION, L"|", new MissingClassPipe(current()))) return false;
            }
            advance(); 
            if (!expect(Token::PUNCTUATION, L"☉", new MissingBrace(current()))) return false;
        }
        else if (match(Token::KEYWORD, L"sectionScore"))
        {
            advance();
            if (!expect(Token::PUNCTUATION, L"∮", new MissingBrace(current()))) return false;

            while (true)
            {
                if (match(Token::IDENTIFIER_CALL))
                {
                    auto ctor = parseCtor();
                    if (!ctor) {
                        synchronize();
                        continue;
                    }
                    if (!symTable.addCtor(PROTECTED, ctor->getConstractor().copy(), current()))
                    {
                        addError(new HowDidYouGetHere(current()));
                    }
                    ctors.emplace_back(PROTECTED, std::move(ctor));
                }
                else
                {
                    unsigned int level = 0;
                    while (true)
                    {
                        if (isAtEnd() || (level == 0 && (match(Token::PUNCTUATION, L"|") || match(Token::PUNCTUATION, L"║")))) break;

                        if (match(Token::PUNCTUATION, L"∮")) level++;
                        else if (match(Token::PUNCTUATION, L"☉")) level--;
                        advance();
                    }
                }

                if (match(Token::PUNCTUATION, L"║"))
                {
                    break;
                }

                if (!expect(Token::PUNCTUATION, L"|", new MissingClassPipe(current()))) return false;
            }
            advance();
            if (!expect(Token::PUNCTUATION, L"☉", new MissingBrace(current()))) return false;
        }
        else
        {
            addError(new UnrecognizedIdentifier(current()));
            synchronize();
        }
    }

    if (isAtEnd())
    {
        addError(new UnexpectedEOF(tokens[len-1]));
        return false;
    }
    return true;
}

std::unique_ptr<Call> Parser::parseFuncCallExpr(const bool isStmt)
{
    const Token& t = current();
    Token nameToken;
    if (!expectAndGet(Token::IDENTIFIER, new MissingIdentifier(current()), nameToken)) return nullptr;
    const std::wstring name = nameToken.value;
    
    std::vector<std::unique_ptr<Expr>> args;

    if (!expect(Token::PUNCTUATION, L"(", new MissingParenthesis(current()))) return nullptr;

    bool first = true;
    while (!match(Token::PUNCTUATION, L")"))
    {
        if (!first)
        {
            if (!expect(Token::PUNCTUATION, L",")) return nullptr;
        }
        first = false;
        auto arg = parseExpr();
        if (!arg) return nullptr;
        args.push_back(std::move(arg));
    }
    advance();

    if (isStmt)
    {
        if (!expect(Token::PUNCTUATION, L"║", new MissingSemicolon(current()))) return nullptr;
    }

    auto expr = std::make_unique<FuncCallExpr>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), name, std::move(args), isStmt);

    callsQ.push(expr.get());

    return expr;
}

std::unique_ptr<Call> Parser::parseCallExpr()
{
    const Token& t = current();
    Token varToken;
    if (!expectAndGet(Token::IDENTIFIER, new MissingIdentifier(current()), varToken)) return nullptr;

    const std::optional<Var> var = symTable.getVar(varToken.value);

    if (!var.has_value())
    {
        addError(new UnrecognizedIdentifier(prev()));
        return nullptr;
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

        pos = savedPos; 
        if (isSlicing)
        {
            call = parseArraySlicingExpr(std::move(call));
        }
        else
        {
            call = parseArrayIndexingExpr(std::move(call));
        }
        if (!call) return nullptr;
    }

    return call;
}

std::unique_ptr<Call> Parser::parseArraySlicingExpr(std::unique_ptr<Call> call)
{
    const Token& t = current();

    std::unique_ptr<Expr> start = std::make_unique<ConstValueExpr>(
        t,
        symTable.getCurrScope(),
        symTable.getCurrFunc(),
        symTable.getCurrClass(),
        std::make_unique<Type>(L"degree"),
        L"0"
    );
    std::unique_ptr<Expr> stop = std::make_unique<ConstValueExpr>(
        t,
        symTable.getCurrScope(),
        symTable.getCurrFunc(),
        symTable.getCurrClass(),
        std::make_unique<Type>(L"degree"),
        L"-1"
    );
    std::unique_ptr<Expr> step = std::make_unique<ConstValueExpr>(
        t,
        symTable.getCurrScope(),
        symTable.getCurrFunc(),
        symTable.getCurrClass(),
        std::make_unique<Type>(L"degree"),
        L"1"
    );

    if (!expect(Token::PUNCTUATION, L"[", new MissingBrace(current()))) return nullptr;

    if (!match(Token::PUNCTUATION, L":"))
    {
        start = parseExpr();
        if (!start) return nullptr;
    }

    if (match(Token::PUNCTUATION, L":"))
    {
        advance();

        if (!match(Token::PUNCTUATION, L":") && !match(Token::PUNCTUATION, L"]"))
        {
            stop = parseExpr();
            if (!stop) return nullptr;
        }


        if (match(Token::PUNCTUATION, L":"))
        {
            advance();

            if (!match(Token::PUNCTUATION, L"]"))
            {
                step = parseExpr();
                if (!step) return nullptr;
            }
        }
    }

    if (!expect(Token::PUNCTUATION, L"]", new MissingBrace(current()))) return nullptr;

    return std::make_unique<ArraySlicingExpr>(
        t,
        symTable.getCurrScope(),
        symTable.getCurrFunc(),
        symTable.getCurrClass(),
        std::move(call),
        std::move(start),
        std::move(stop),
        std::move(step)
    );
}

std::unique_ptr<Call> Parser::parseArrayIndexingExpr(std::unique_ptr<Call> call)
{
    const Token& t = current();
    if (!expect(Token::PUNCTUATION, L"[", new MissingBrace(current()))) return nullptr;
    std::unique_ptr<Expr> index = parseExpr();
    if (!index) return nullptr;
    if (!expect(Token::PUNCTUATION, L"]", new MissingBrace(current()))) return nullptr;

    return std::make_unique<ArrayIndexingExpr>(
        t,
        symTable.getCurrScope(),
        symTable.getCurrFunc(),
        symTable.getCurrClass(),
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
    if (match(Token::TYPE)) return parseType();
    return parseClassType();
}

std::unique_ptr<Type> Parser::parseType()
{
    Token typeToken;
    if (!expectAndGet(Token::TYPE, new InvalidNumberLiteral(current()), typeToken)) return nullptr;
    std::wstring value = typeToken.value;

    if (value == L"sharp freq" || value == L"flat freq")
    {
        addError(new UnrecognizedToken(typeToken));
        return nullptr;
    }

    return std::make_unique<Type>(value);
}

std::unique_ptr<ArrayType> Parser::parseArrayType()
{
    std::unique_ptr<IType> arrType = parseIType();
    if (!arrType) return nullptr;

    return std::make_unique<ArrayType>(std::move(arrType));
}

std::unique_ptr<ClassType> Parser::parseClassType()
{
    const std::wstring classname = current().value;
    if (!expect(Token::IDENTIFIER, new MissingBrace(current()))) return nullptr;

    if (auto cls = SymbolTable::getClass(classname))
    {
        return std::make_unique<ClassType>(cls);
    }

    return nullptr;
}


std::unique_ptr<Expr> Parser::parseExpr(const bool hasParens, const bool isStmt, const bool allowBackslash)
{
    if (match(Token::IDENTIFIER) && peek().type == Token::OP_UNARY)
    {
        return parseUnaryOpExpr();
    }

    auto left = parsePrimary(isStmt, allowBackslash);
    if (!left) return nullptr;

    auto expr = parseBinaryOpRight(0, std::move(left), isStmt, allowBackslash);
    if (!expr) return nullptr;

    if (hasParens)
    {
        expr->setHasParens(true);
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parsePrimary(const bool isStmt, const bool allowBackslash)
{
    const Token& t = current();

    if (t.value == L"+" || t.value == L"-" ||
         t.value == L"*" || t.value == L"/" ||
         t.value == L"//")
    {
        std::wstring op = t.value;
        advance(); 

        auto right = parseExpr(false, false, allowBackslash);
        if (!right) return nullptr;

        return std::make_unique<BinaryOpExpr>(
            t,
            symTable.getCurrScope(),
            symTable.getCurrFunc(),
            symTable.getCurrClass(),
            op,
            nullptr,
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
            return parseFuncCallExpr(isStmt);
        }
        return parseCallExpr();
    }


    if (match(Token::PUNCTUATION, L"("))
    {
        advance();
        auto expr = parseExpr(true, false, allowBackslash);
        if (!expr) return nullptr;
        if (!expect(Token::PUNCTUATION, L")", new MissingParenthesis(current()))) return nullptr;
        return expr;
    }
    
    addError(new UnexpectedToken(current()));
    return nullptr;
}

std::unique_ptr<Expr> Parser::parseBinaryOpRight(int exprPrec, std::unique_ptr<Expr> left, const bool isStmt, const bool allowBackslash)
{
    while (true)
    {
        if (!(match(Token::OP_BINARY) || (allowBackslash && match(Token::PUNCTUATION, L"\\"))))
            return left;

        const Token& t = current();
        std::wstring op = t.value;
        int prec = BinaryOpExpr::getPrecedence(op);

        if (prec < exprPrec)
            return left;

        advance();

        auto right = parsePrimary(isStmt, allowBackslash);
        if (!right) return nullptr;

        if (match(Token::OP_BINARY) || (allowBackslash && match(Token::PUNCTUATION, L"\\")))
        {
            int nextPrec = BinaryOpExpr::getPrecedence(current().value);
            if (nextPrec > prec)
            {
                right->setIsStmt(false);
                right = parseBinaryOpRight(prec + 1, std::move(right), isStmt, allowBackslash);
                if (!right) return nullptr;
            }
        }

        if (op == L"\\")
        {
            auto callLeft = std::unique_ptr<Call>(
                dynamic_cast<Call*>(left.release())
            );

            auto callRight = std::unique_ptr<Call>(
                dynamic_cast<Call*>(right.release())
            );

            if (callLeft == nullptr || callRight == nullptr) {
                addError(new HowDidYouGetHere(current()));
                return nullptr;
            }

            callRight->setIsClassItem(true);

            left = std::make_unique<DotOpExpr>(
                t,
                symTable.getCurrScope(),
                symTable.getCurrFunc(),
                symTable.getCurrClass(),
                op,
                std::move(callLeft),
                std::move(callRight)
            );
        }
        else
        {
            left = std::make_unique<BinaryOpExpr>(
                t,
                symTable.getCurrScope(),
                symTable.getCurrFunc(),
                symTable.getCurrClass(),
                op,
                std::move(left),
                std::move(right)
            );
        }
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
        addError(new InvalidExpression(t));
        return nullptr;
    }

    advance();
    return std::make_unique<ConstValueExpr>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::make_unique<Type>(type), t.value);
}

std::unique_ptr<UnaryOpExpr> Parser::parseUnaryOpExpr(const bool isStmt)
{
    const Token& t = current();
    std::unique_ptr<Call> call = parseCallExpr();
    if (!call) return nullptr;
    UnaryOp op;

    Token opToken;
    if (!expectAndGet(Token::OP_UNARY, opToken)) return nullptr;
    const std::wstring value = opToken.value;


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
        addError(new UnexpectedToken(current()));
        return nullptr;
    }

    if (!expect(Token::PUNCTUATION, L"║", new MissingSemicolon(current()))) return nullptr;

    return std::make_unique<UnaryOpExpr>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(call), op, isStmt);
}
