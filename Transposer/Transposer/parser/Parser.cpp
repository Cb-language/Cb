#include "Parser.h"

#include <memory>
#include <ranges>
#include <vector>
#include <filesystem>
#include <algorithm>

// ---------- AST related ----------
#include "AST/statements/FuncDeclStmt.h"
#include "AST/statements/expression/FuncCallExpr.h"

// ---------- syntax errors ----------
#include "../errorHandling/syntaxErrors/UnexpectedToken.h"
#include "../errorHandling/syntaxErrors/MissingSemicolon.h"
#include "../errorHandling/syntaxErrors/ExpectedAPath.h"

// ---------- semantic errors ----------
#include "../errorHandling/semanticErrors/InvalidPathExtension.h"

#include "files/FileGraph.h"
#include "symbols/Type/ClassType.h"

Parser::Parser(const std::queue<Token>& tokens) : c(tokens), stmtParser(c, typeParser, exprParser), exprParser(c, typeParser), typeParser(c)
{
}

const ParserContext& Parser::getContext() const
{
    return c;
}

std::vector<std::pair<std::filesystem::path, Token>> Parser::readIncludes()
{
    std::vector<std::pair<std::filesystem::path, Token>> v;
    if (c.getIncludes().empty())
    {
        while (c.matchConsume(CbTokenType::KEYWORD_FEAT))
        {
            Token pathToken;
            if (!c.expect(CbTokenType::CONST_STR,std::make_unique<ExpectedAPath>(c.current()), pathToken)) return {};

            std::string str = pathToken.value.value();
            str.erase(
                std::ranges::remove(str, L'"').begin(),
                str.end()
            );
            std::filesystem::path path = str;
            if (!c.matchNonConsume(CbTokenType::PUNCTUATION_SEMICOLON))
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
                    c.getFuncDecl(),
                    path,
                    c.getClassDecl()
                )
            );

            v.emplace_back(path, pathToken);
        }
    }

    return v;
}

void Parser::parse()
{
    stmtParser.parse();
}

void Parser::analyze()
{
    for (const auto& stmt : c.getStmts())
    {
        try
        {
            stmt->analyze();
        }
        catch (const Error& e)
        {
            c.addError(std::make_unique<Error>(e.getToken(), e.getErrorMessage()));
        }
    }
}

std::string Parser::translateToCpp(const std::filesystem::path& hPath, const bool isMain) const
{
    return "";
}

std::string Parser::translateToH(const bool isMain) const
{
    return "";
}

// bool Parser::shouldProduceCpp(const bool isMain) const
// {
//     if (isMain || c.getHasMain()) return true;
//
//     for (const auto& stmt : c.getStmts())
//     {
//         if (dynamic_cast<IncludeStmt*>(stmt.get())) continue;
//
//         if (const auto classStmt = dynamic_cast<ClassDeclStmt*>(stmt.get()))
//         {
//             // if (classStmt->getCurrClass() && !classStmt->getCurrClass()->isAbstract())
//             // {
//             // return true; // Found a concrete class
//             // }
//             return true;
//         }
//         else if (dynamic_cast<FuncDeclStmt*>(stmt.get()))
//         {
//             return true; // Found a global function implementation
//         }
//         else
//         {
//             // Any other statement in global scope (e.g. VarDeclStmt) needs a .cpp
//             return true;
//         }
//     }
//
//     // If it only has includes or abstract classes, don't produce a .cpp
//     return false;
// } TODO add to output stage
