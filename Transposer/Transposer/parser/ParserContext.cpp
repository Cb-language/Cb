#include "ParserContext.h"

#include "errorHandling/lexicalErrors/UnexpectedEOF.h"

ParserContext::ParserContext(const std::vector<Token>& tokens) : tokens(tokens), len(tokens.size()), pos(0), hasMain(false), funcDecl(nullptr), classDecl(nullptr)
{
}

void ParserContext::addError(std::unique_ptr<Error> err)
{
    errors.emplace_back(std::move(err));
}

const Token& ParserContext::current() const
{
    return tokens[pos];
}

Token ParserContext::advance()
{
    return tokens[pos++];
}

const Token& ParserContext::peek() const // TODO wrap the parser calling function in try catch
{
    if (pos >= len - 1) throw UnexpectedEOF(current());
    return tokens[pos + 1];
}

bool ParserContext::isAtEnd() const
{
    return pos >= len;
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

bool ParserContext::isAssignmentStmtAhead()
{
    const size_t startPos = pos;

    // Must start with identifier
    if (!matchNonConsume(TokenType::IDENTIFIER))
    {
        return false;
    }

    advance(); // consume IDENTIFIER

    while (true)
    {
        // Handle member access: \identifier
        if (matchNonConsume(TokenType::PUNCTUATION_BACKSLASH))
        {
            advance(); // consume '\'

            if (!matchNonConsume(TokenType::IDENTIFIER))
            {
                pos = startPos;
                return false;
            }

            advance(); // consume IDENTIFIER
        }
        // Handle indexing: [...]
        else if (matchNonConsume(TokenType::PUNCTUATION_OPEN_SQUARE_BRACE))
        {
            advance(); // consume '['

            int depth = 1;
            while (depth > 0)
            {
                if (matchNonConsume(TokenType::PUNCTUATION_OPEN_SQUARE_BRACE))
                {
                    depth++;
                }
                else if (matchNonConsume(TokenType::PUNCTUATION_CLOSE_SQUARE_BRACE))
                {
                    depth--;
                }

                advance();
            }
        }
        else
        {
            break;
        }
    }

    const bool result =
        isAssignmentOp() ||
        matchNonConsume(TokenType::PUNCTUATION_SEMICOLON);

    pos = startPos;
    return result;
}

bool ParserContext::isUnaryOpStmtAhead()
{
    const size_t startPos = pos;

    if (!matchNonConsume(TokenType::IDENTIFIER))
    {
        return false;
    }

    advance(); // consume IDENTIFIER

    while (true)
    {
        // Handle member access: \identifier
        if (matchNonConsume(TokenType::PUNCTUATION_BACKSLASH))
        {
            advance(); // consume '\'

            if (!matchNonConsume(TokenType::IDENTIFIER))
            {
                pos = startPos;
                return false;
            }

            advance(); // consume IDENTIFIER
        }

        // Handle indexing: [...]
        else if (matchNonConsume(TokenType::PUNCTUATION_OPEN_SQUARE_BRACE))
        {
            advance(); // consume '['

            int depth = 1;

            while (depth > 0)
            {
                if (matchNonConsume(TokenType::PUNCTUATION_OPEN_SQUARE_BRACE))
                {
                    depth++;
                }

                else if (matchNonConsume(TokenType::PUNCTUATION_CLOSE_SQUARE_BRACE))
                {
                    depth--;
                }

                advance();
            }
        }

        else
        {
            break;
        }
    }

    bool result = isUnaryOp();
    pos = startPos;

    return result;
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

const Token& ParserContext::getLast() const
{
    return tokens[len - 1];
}

const std::vector<std::unique_ptr<Stmt>>& ParserContext::getStmts() const
{
    return stmts;
}

const std::vector<std::unique_ptr<Error>>& ParserContext::getErrors() const
{
    return errors;
}

const std::vector<Token>& ParserContext::getTokens() const
{
    return tokens;
}

size_t ParserContext::getPos() const
{
    return pos;
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