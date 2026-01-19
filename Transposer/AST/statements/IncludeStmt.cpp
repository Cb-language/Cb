#include "IncludeStmt.h"

IncludeStmt::IncludeStmt(const Token &token, Scope *scope, FuncDeclStmt *funcDecl,
                         const std::filesystem::path &path) : Stmt(token, scope, funcDecl), path(path)
{
}

void IncludeStmt::analyze() const
{

}

std::string IncludeStmt::translateToCpp() const
{
    return "#include \"" + path.string() + "\"";
}
