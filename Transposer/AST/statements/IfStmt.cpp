#include "IfStmt.h"

IfStmt::IfStmt(Scope *scope, FuncDeclStmt *funcDecl, std::unique_ptr<Expr> expr, std::unique_ptr<Stmt> body,
               std::unique_ptr<Stmt> elseIfStmt, const bool isElseIf) : Stmt(scope, funcDecl), expr(std::move(expr)), body(std::move(body)), elseIfStmt(std::move(elseIfStmt)), isElseIf(isElseIf)
{
}

bool IfStmt::isLegal() const
{
    return body->isLegal() && expr->isLegal();
}

std::string IfStmt::translateToCpp() const
{
    std::ostringstream oss;
    oss << getTabs() <<  "if (" << expr->translateToCpp() << ")\n" << body->translateToCpp();
    if (elseIfStmt != nullptr)
    {
        oss << "\n" << getTabs();
        if (isElseIf)
        {
            oss.clear();
            oss << "else " << elseIfStmt->translateToCpp();
        }
        else
        {
            oss << "else\n" << elseIfStmt->translateToCpp();
        }
    }

    return oss.str();
}