#include "Parser.h"

#include <memory>
#include <ranges>
#include <vector>
#include <filesystem>
#include <algorithm>

// ---------- syntax errors ----------
#include "../errorHandling/syntaxErrors/UnexpectedToken.h"
#include "../errorHandling/syntaxErrors/MissingSemicolon.h"
#include "../errorHandling/syntaxErrors/ExpectedAPath.h"

// ---------- semantic errors ----------
#include "../errorHandling/semanticErrors/InvalidPathExtension.h"
#include "errorHandling/lexicalErrors/UnexpectedEOF.h"

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
    if (c.isEmpty())
    {
        return {};
    }
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
    try
    {
        stmtParser.parse();
    }
    catch (UnexpectedEOF& e)
    {
        c.addError(std::make_unique<UnexpectedEOF>(e.getToken()));
    }
}

std::string Parser::translateToCpp(const std::filesystem::path& hPath, const bool isMain)
{
    std::ostringstream oss;

    oss << "#include <iostream>" << std::endl;

    if (isMain) oss << translateToH(isMain);
    else oss << "#include \"" << hPath.generic_string() << "\"" << std::endl;

    for (const auto& stmt : c.getStmts())
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

std::string Parser::translateToH(const bool isMain)
{
    std::ostringstream oss;
    if (!isMain) oss << "#pragma once" << std::endl;

    oss << "#include <string>" << std::endl;
    oss << Utils::getAllObjIncludes() << std::endl;

    if (!c.getIncludes().empty()) oss << std::endl;

    for (const auto& i : c.getIncludes())
    {
        oss << i->translateToCpp() << std::endl;
    }

    for (const auto& stmt : c.getStmts()) oss << stmt->translateToH();

    return oss.str();
}

void Parser::addError(std::unique_ptr<Error> err)
{
    c.addError(std::move(err));
}