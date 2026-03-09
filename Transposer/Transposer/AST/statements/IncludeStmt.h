#pragma once

#include <filesystem>
#include "AST/abstract/Statement.h"

class IncludeStmt : public Stmt
{
private:
    std::filesystem::path inPath;
    std::filesystem::path outPath;
public:
    IncludeStmt(const Token& token, IFuncDeclStmt* funcDecl, const std::filesystem::path& inPath, ClassDeclStmt* classDecl = nullptr);

    void analyze() const override;
    std::string translateToCpp() const override;

    const std::filesystem::path& getOutPath() const;
};
