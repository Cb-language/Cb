#include "Parser.h"

#include <algorithm>

#include "errorHandling/semanticErrors/InvalidPathExtension.h"
#include "errorHandling/syntaxErrors/ExpectedAPath.h"
#include "errorHandling/syntaxErrors/MissingSemicolon.h"

Parser::Parser(const std::vector<Token>& tokens) : c(tokens), stmtParser(c), exprParser(c), typeParser(c)
{
}

const ParserContext& Parser::getContext() const
{
    return c;
}

ParserContext& Parser::getContext()
{
    return c;
}

std::vector<std::pair<std::filesystem::path, Token>> Parser::readIncludes()
{
    std::vector<std::pair<std::filesystem::path, Token>> v;
    if (c.getIncludes().empty() && c.getPos() == 0)
    {
        while (c.match(TokenType::KEYWORD_FEAT))
        {
            c.advance();

            Token pathToken;
            if (!c.expect(TokenType::CONST_STR,std::make_unique<ExpectedAPath>(c.current()), pathToken)) return {};

            std::string str = pathToken.value.value();
            str.erase(
                std::ranges::remove(str, L'"').begin(),
                str.end()
            );
            std::filesystem::path path = str;
            if (!c.match(TokenType::PUNCTUATION_SEMICOLON))
            {
                c.addError(std::make_unique<MissingSemicolon>(c.current()));
                return {};
            }
            c.advance(); // eat the semicolon

            if (path.extension() != ".cb")
            {
                c.addError(std::make_unique<InvalidPathExtension>(c.current()));
                return {};
            }

            c.getIncludes().emplace_back(
                std::make_unique<IncludeStmt>(
                    pathToken,
                    c.getSymTable().getCurrScope(),
                    c.getSymTable().getCurrFunc(),
                    c.getSymTable().getCurrClass(),
                    path
                )
            );

            v.emplace_back(path, pathToken);
        }
    }

    return v;
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
    if (isMain || c.getHasMain()) return true;

    for (const auto& stmt : c.getStmts())
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
