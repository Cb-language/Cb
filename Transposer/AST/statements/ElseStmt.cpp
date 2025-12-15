#include "ElseStmt.h"

ElseStmt::ElseStmt(Scope *scope, FuncDeclStmt *funcDecl, std::unique_ptr<BodyStmt> &body) : Stmt(scope, funcDecl), body(std::move(body))
{
}

bool ElseStmt::isLegal() const
{
    return body->isLegal();
}

std::string ElseStmt::translateToCpp() const
{
    return getTabs() + "else\n" + body->translateToCpp();
}
