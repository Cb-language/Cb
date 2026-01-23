#include "IncludeStmt.h"

#include "files/FileGraph.h"

IncludeStmt::IncludeStmt(const Token &token, Scope *scope, FuncDeclStmt *funcDecl,
                         const std::filesystem::path &inPath) : Stmt(token, scope, funcDecl), inPath(inPath), outPath(FileGraph::getOutPath(inPath))
{
}

void IncludeStmt::analyze() const
{

}

std::string IncludeStmt::translateToCpp() const
{
    return "#include \"" + outPath.string() + "\"\n";
}
