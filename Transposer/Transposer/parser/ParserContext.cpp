#include "ParserContext.h"

#include <functional>

#include "errorHandling/lexicalErrors/UnexpectedEOF.h"
#include "errorHandling/syntaxErrors/UnexpectedToken.h"

ParserContext::ParserContext(const std::queue<Token>& tokens) : tokens(tokens), len(tokens.size()), funcDecl(nullptr), hasMain(false)
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

Token ParserContext::advance()
{
    auto t = tokens.front();
    tokens.pop();
    return std::move(t);
}

Token ParserContext::copyCurrent()
{
    if (tokens.empty()) throw UnexpectedEOF(current());
    return tokens.front();
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

bool ParserContext::isAssignmentOp() const
{
    return (current().type >= CbTokenType::ASSIGNMENT_OP_EQUAL && current().type <= CbTokenType::ASSIGNMENT_OP_MODULUS_EQUAL);
}

bool ParserContext::isBinaryOp() const
{
    return (current().type >= CbTokenType::BINARY_OP_EQUIAL && current().type <= CbTokenType::BINARY_OP_AND);
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

IFuncDeclStmt* ParserContext::getFuncDecl() const
{
    return funcDecl;
}

ClassDeclStmt* ParserContext::getClassDecl() const
{
    if (classDecl.empty()) return nullptr;
    return &classDecl.top().get();
}

void ParserContext::pushClassDecl(ClassDeclStmt& decl)
{
    classDecl.push(decl);
}

void ParserContext::popClassDecl()
{
    classDecl.pop();
}

void ParserContext::setFuncDecl(IFuncDeclStmt* funcDecl)
{
    this->funcDecl = funcDecl;
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