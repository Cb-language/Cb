#pragma once
#include <vector>
#include <memory>

#include "ExprParser.h"
#include "ParserContext.h"
#include "StmtParser.h"
#include "TypeParser.h"
#include "AST/abstract/Statement.h"
#include "AST/statements/expression/FuncCallExpr.h"
#include "other/SymbolTable.h"
#include "AST/statements/IncludeStmt.h"

class Parser
{
private:
    ParserContext c;

    StmtParser stmtParser;
    ExprParser exprParser;
    TypeParser typeParser;

public:
    explicit Parser(const std::queue<Token>& tokens);

    const ParserContext& getContext() const;

    std::vector<std::pair<std::filesystem::path, Token>> readIncludes();
    void parse();
    std::string translateToCpp(const std::filesystem::path& hPath, const bool isMain = false);
    std::string translateToH(const bool isMain = false);
    void addError(std::unique_ptr<Error> err);
    //bool shouldProduceCpp(bool isMain) const;
};

