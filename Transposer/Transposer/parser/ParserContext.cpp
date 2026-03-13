#include "ParserContext.h"

#include <functional>

#include "errorHandling/lexicalErrors/UnexpectedEOF.h"
#include "errorHandling/syntaxErrors/MissingSemicolon.h"
#include "errorHandling/syntaxErrors/NoLineOpener.h"
#include "errorHandling/syntaxErrors/NoRest.h"
#include "errorHandling/syntaxErrors/UnexpectedToken.h"

ParserContext::ParserContext(const std::queue<Token>& tokens)
    : tokens(tokens), len(tokens.size()), firstToken(CbTokenType::ERROR_TOKEN, std::nullopt, 0, 0, ""), lastToken(CbTokenType::ERROR_TOKEN, std::nullopt, 0, 0, ""), breakables(0),
      continueables(0), isNewLine(false), isInFunc(false)
{
    if (!this->tokens.empty())
    {
        firstToken = this->tokens.front();
        lastToken = firstToken;
    }
}

void ParserContext::addError(std::unique_ptr<Error> err)
{
    errors.emplace_back(std::move(err));
}

const Token& ParserContext::current()
{
    if (tokens.empty())
    {
        addError(std::make_unique<UnexpectedEOF>(lastToken));
        return lastToken;
    }
    return tokens.front();
}

Token ParserContext::copyCurrent()
{
    if (tokens.empty()) throw UnexpectedEOF(current());
    return tokens.front();
}

Token ParserContext::advance()
{
    if (tokens.empty()) throw UnexpectedEOF(lastToken);

    auto t = std::move(tokens.front());
    tokens.pop();
    lastToken = t;

    if (t.type == CbTokenType::PUNCTUATION_CLOSE_FUNC)
    {
        setIsInFunc(false);
    }

    if (!isNewLine)
    {
        isNewLine = true;
        while (!tokens.empty() && tokens.front().type == CbTokenType::PUNCTUATION_NEW_LINE)
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
    if (tokens.empty()) addError(std::make_unique<UnexpectedEOF>(current()));
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

bool ParserContext::matchNonConsume(const CbTokenType type)
{
    if (tokens.empty()) addError(std::make_unique<UnexpectedEOF>(current()));
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

bool ParserContext::isUnaryOp()
{
    return (current().type >= CbTokenType::UNARY_OP_SHARP && current().type <= CbTokenType::UNARY_OP_NATRUAL);
}

bool ParserContext::isBinaryOp()
{
    return (current().type >= CbTokenType::BINARY_OP_EQUAL && current().type <= CbTokenType::BINARY_OP_AND);
}

bool ParserContext::isType() const
{
    if (tokens.empty()) return false;
    return (tokens.front().type >= CbTokenType::TYPE_FLAT && tokens.front().type <= CbTokenType::TYPE_FERMATA);
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

void ParserContext::addBreakable()
{
    this->breakables++;
}

void ParserContext::removeBreakable()
{
    this->breakables--;
}

void ParserContext::addContinueable()
{
    this->continueables++;
}

void ParserContext::removeContinueable()
{
    this->continueables--;
}

bool ParserContext::getIsBreakable() const
{
    return breakables != 0;
}

bool ParserContext::getIsContinueable() const
{
    return continueables != 0;
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