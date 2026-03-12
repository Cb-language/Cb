#include "ParserContext.h"

#include <functional>

#include "StmtParser.h"
#include "errorHandling/lexicalErrors/UnexpectedEOF.h"
#include "errorHandling/syntaxErrors/MissingSemicolon.h"
#include "errorHandling/syntaxErrors/NoLineOpener.h"
#include "errorHandling/syntaxErrors/NoRest.h"
#include "errorHandling/syntaxErrors/UnexpectedToken.h"

ParserContext::ParserContext(const std::queue<Token>& tokens)
    : tokens(tokens), len(tokens.size()), hasMain(false), isNewLine(false), isInFunc(false)
{
    if (!tokens.empty())
    {
        firstToken = tokens.front();
    }
    else
    {
        firstToken = Token(CbTokenType::PUNCTUATION_NEW_LINE, std::nullopt, 0, 0, "");
    }
}

void ParserContext::addError(std::unique_ptr<Error> err)
{
    errors.emplace_back(std::move(err));
}

const Token& ParserContext::current() const
{
    if (tokens.empty()) throw UnexpectedEOF(getFirstToken());
    return tokens.front();
}

Token ParserContext::copyCurrent()
{
    if (tokens.empty()) throw UnexpectedEOF(current());
    return tokens.front();
}

Token ParserContext::advance()
{

    auto t = tokens.front();
    tokens.pop();

    if (!isNewLine)
    {
        isNewLine = true;

        while (!tokens.empty() && current().type == CbTokenType::PUNCTUATION_NEW_LINE)
        {
            if (isInFunc)
            {
                eatFuncNewLine();
            }
            else
            {
                advance();
            }
        }

        isNewLine = false;
    }

    return std::move(t);
}

void ParserContext::expectSemiColon()
{
    if (!isInFunc)
    {
        if (!matchConsume(CbTokenType::PUNCTUATION_SEMICOLON))
        {
            addError(std::make_unique<MissingSemicolon>(copyCurrent()));
        }
        return;
    }

    if (matchConsume(CbTokenType::PUNCTUATION_CLOSE_FUNC))
    {
        setIsInFunc(false);
    }
    else
    {
        expect(CbTokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(copyCurrent()));
    }
}

void ParserContext::eatFuncNewLine()
{
    if (!matchConsume(CbTokenType::PUNCTUATION_NEW_LINE))
    {
        return;
    }
    expect(CbTokenType::PUNCTUATION_OPEN_LINE, std::make_unique<NoLineOpener>(copyCurrent()));

    if (matchConsume(CbTokenType::PUNCTUATION_SEMICOLON) || matchConsume(CbTokenType::PUNCTUATION_CLOSE_FUNC))
    {
        addError(std::make_unique<NoRest>(copyCurrent()));
    }
    eatRest();
}

void ParserContext::eatRest()
{
    if (!matchConsume(CbTokenType::PUNCTUATION_REST))
    {
        return;
    }
    expectSemiColon();
}


bool ParserContext::matchConsume(const CbTokenType type, const std::optional<std::reference_wrapper<Token>> out)
{
    if (current().type == type)
    {
        if (out.has_value())
        {
            out.value().get() = current();
        }
        advance();
        return true;
    }
    return false;
}

bool ParserContext::matchNonConsume(const CbTokenType type) const
{
    return current().type == type;
}

bool ParserContext::expect(const CbTokenType type, std::unique_ptr<Error> err, const std::optional<std::reference_wrapper<Token>> out)
{
    if (!matchNonConsume(type))
    {
        if (err)
            addError(std::move(err));

        return false;
    }

    if (out.has_value())
        out->get() = current();

    advance();
    return true;
}

FQN ParserContext::parseFQN()
{
    FQN res;
    do
    {
        Token iden;
        if (!matchConsume(CbTokenType::IDENTIFIER, iden))
        {
            addError(std::make_unique<UnexpectedToken>(current()));
            return res;
        }
        res.emplace_back(iden.value.value());

    } while (matchConsume(CbTokenType::PUNCTUATION_BACKSLASH));
    return res;
}

bool ParserContext::isUnaryOp() const
{
    return (current().type >= CbTokenType::UNARY_OP_SHARP && current().type <= CbTokenType::UNARY_OP_NATRUAL);
}

bool ParserContext::isBinaryOp() const
{
    return (current().type >= CbTokenType::BINARY_OP_EQUAL && current().type <= CbTokenType::BINARY_OP_AND);
}

bool ParserContext::isType() const
{
    return (current().type >= CbTokenType::TYPE_FLAT && current().type <= CbTokenType::TYPE_FERMATA);
}

bool ParserContext::getHasMain() const
{
    return hasMain;
}

const std::vector<std::unique_ptr<Stmt>>& ParserContext::getStmts() const
{
    return stmts;
}

const std::vector<std::unique_ptr<Error>>& ParserContext::getErrors() const
{
    return errors;
}

void ParserContext::setIsInFunc(const bool isInFunc)
{
    this->isInFunc = isInFunc;
}

bool ParserContext::getIsInFunc() const
{
    return isInFunc;
}

bool ParserContext::isEmpty() const
{
    return tokens.empty();
}

const Token& ParserContext::getFirstToken() const
{
    return firstToken;
}

std::vector<std::unique_ptr<Stmt>>& ParserContext::getStmts()
{
    return stmts;
}

std::vector<std::unique_ptr<Error>>& ParserContext::getErrors()
{
    return errors;
}

std::vector<std::unique_ptr<IncludeStmt>>& ParserContext::getIncludes()
{
    return includes;
}