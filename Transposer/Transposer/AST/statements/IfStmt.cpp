#include "IfStmt.h"

IfStmt::IfStmt(const Token& token, Scope *scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass, std::unique_ptr<Expr> expr, std::unique_ptr<Stmt> body,
               std::unique_ptr<Stmt> elseIfStmt, const bool isElseIf)
               : Stmt(token, scope, funcDecl, currClass), expr(std::move(expr)), body(std::move(body)), elseIfStmt(std::move(elseIfStmt)), isElseIf(isElseIf)
{
}

void IfStmt::analyze() const
{
    body->analyze();
    expr->analyze();
}

std::string IfStmt::translateToCpp() const
{
    std::ostringstream oss;
    oss << getTabs() <<  "if (" << expr->translateToCpp() << ")" << std::endl;
    oss << body->translateToCpp();
    if (elseIfStmt != nullptr)
    {
        std::string elseStr = elseIfStmt->translateToCpp();
        oss << "\n" << getTabs();
        if (isElseIf)
        {
            oss.clear();
            oss << "else " << Utils::removeFirstTabs(elseStr);
        }
        else
        {
            oss << "else\n" << elseStr;
        }
    }

    return oss.str();
}