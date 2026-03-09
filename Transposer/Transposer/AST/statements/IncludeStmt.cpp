#include "IncludeStmt.h"

#include "files/FileGraph.h"

IncludeStmt::IncludeStmt(const Token& token, IFuncDeclStmt* funcDecl,
                         const std::filesystem::path& inPath, ClassDeclStmt* classDecl) : Stmt(token, funcDecl, classDecl), inPath(inPath),
                         outPath((File::getOutDir() / inPath.filename()).replace_extension("h"))
{
}

void IncludeStmt::analyze() const
{

}

std::string IncludeStmt::translateToCpp() const
{
    return "#include \"" + Utils::normalizePath(outPath) + "\"";
}

const std::filesystem::path& IncludeStmt::getOutPath() const
{
    return outPath;
}
