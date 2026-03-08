#include "Parser.h"

Parser::Parser(const std::vector<Token>& tokens) : context(tokens)
{
}

const ParserContext& Parser::getContext() const
{
    return context;
}

ParserContext& Parser::getContext()
{
    return context;
}

std::vector<std::pair<std::filesystem::path, Token>> Parser::readIncludes()
{
    return {};
}

void Parser::parse()
{
}

void Parser::analyze()
{
}

std::string Parser::translateToCpp(const std::filesystem::path& hPath, const bool isMain) const
{
    return "";
}

std::string Parser::translateToH(const bool isMain) const
{
    return "";
}

bool Parser::shouldProduceCpp(bool isMain) const
{
    if (isMain || context.getHasMain()) return true;

    for (const auto& stmt : context.getStmts())
    {
        if (dynamic_cast<IncludeStmt*>(stmt.get())) continue;

        if (auto classStmt = dynamic_cast<ClassDeclStmt*>(stmt.get()))
        {
            if (classStmt->getCurrClass() && !classStmt->getCurrClass()->isAbstract())
            {
                return true; // Found a concrete class
            }
        }
        else if (dynamic_cast<FuncDeclStmt*>(stmt.get()))
        {
            return true; // Found a global function implementation
        }
        else
        {
            // Any other statement in global scope (e.g. VarDeclStmt) needs a .cpp
            return true;
        }
    }

    // If it only has includes or abstract classes, don't produce a .cpp
    return false;
}
