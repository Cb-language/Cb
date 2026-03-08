#include "ParserContext.h"

ParserContext::ParserContext(const std::vector<Token>& tokens) : tokens(tokens), len(tokens.size()), pos(0), hasMain(false)
{
}

void ParserContext::addToSymTable(const SymbolTable& symTable)
{
    this->symTable += symTable;
}

const SymbolTable& ParserContext::getSymTable() const
{
    return symTable;
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

SymbolTable& ParserContext::getSymTable()
{
    return symTable;
}

std::vector<std::unique_ptr<Stmt>>& ParserContext::getStmts()
{
    return stmts;
}

std::vector<std::unique_ptr<Error>>& ParserContext::getErrors()
{
    return errors;
}
