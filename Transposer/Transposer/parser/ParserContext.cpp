#include "ParserContext.h"

#include "errorHandling/lexicalErrors/UnexpectedEOF.h"

ParserContext::ParserContext(const std::queue<Token>& tokens) : tokens(tokens), len(tokens.size()), funcDecl(nullptr), hasMain(false)
{
    if (!tokens.empty())
    {
        firstToken = tokens.front();
    }
    else
    {
        firstToken = Token(TokenType::PUNCTUATION_NEW_LINE, std::nullopt, 0, 0, "");
    }
}

void ParserContext::addError(std::unique_ptr<Error> err)
{
    errors.emplace_back(std::move(err));
}

const Token& ParserContext::current() const
{
    if (tokens.empty())
    {
        throw UnexpectedEOF(getFirstToken());
    }
    return tokens.front();
}

Token ParserContext::advance()
{
    auto t = tokens.front();
    tokens.pop();
    return std::move(t);
}

const Token& ParserContext::peek() const // TODO wrap the parser calling function in try catch
{
    if (tokens.empty()) throw UnexpectedEOF(current());
    return tokens.front();
}

bool ParserContext::matchConsume(const TokenType type, const std::optional<std::reference_wrapper<Token>> out)
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

bool ParserContext::matchNonConsume(const TokenType type) const
{
    return current().type == type;
}

bool ParserContext::expect(const TokenType type, std::unique_ptr<Error> err, const std::optional<std::reference_wrapper<Token>> out)
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

bool ParserContext::isUnaryOp() const
{
    return (current().type >= TokenType::UNARY_OP_SHARP && current().type <= TokenType::UNARY_OP_NATRUAL);
}

bool ParserContext::isAssignmentOp() const
{
    return (current().type >= TokenType::ASSIGNMENT_OP_EQUAL && current().type <= TokenType::ASSIGNMENT_OP_MODULUS_EQUAL);
}

bool ParserContext::isBinaryOp() const
{
    return (current().type >= TokenType::BINARY_OP_EQUIAL && current().type <= TokenType::BINARY_OP_AND);
}

bool ParserContext::isType() const
{
    return (current().type >= TokenType::TYPE_FLAT && current().type <= TokenType::TYPE_FERMATA);
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

std::stack<std::reference_wrapper<ClassDeclStmt>> ParserContext::getClassDecl() const
{
    return classDecl;
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