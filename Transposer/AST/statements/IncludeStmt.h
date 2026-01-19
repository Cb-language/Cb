#pragma once

#include <filesystem>
#include "AST/abstract/Statement.h"

class IncludeStmt : public Stmt
{
private:
    std::filesystem::path path;
public:
    IncludeStmt(const Token& token, Scope* scope, FuncDeclStmt* funcDecl, const std::filesystem::path& path);

    void analyze() const override;
    std::string translateToCpp() const override;
};
