#include "IfStmt.h"

IfStmt::IfStmt(const Token& token, StmtWithBody ifStmt, std::vector<StmtWithBody>& elseStmts) : Stmt(token), ifStmt(std::move(ifStmt))
{
    for (auto& stmt : elseStmts)
    {
        this->elseStmts.push_back(std::move(stmt));
    }
}

void IfStmt::analyze() const
{
    ifStmt.body->analyze();
    ifStmt.expr->analyze();
}

std::string IfStmt::translateToCpp() const
{
    std::ostringstream oss;
    oss << getTabs() <<  "if (" << ifStmt.expr->translateToCpp() << ")" << std::endl;
    oss << ifStmt.body->translateToCpp();

    for (auto& stmt : elseStmts)
    {
        if (stmt.expr != nullptr)
        {
            oss << "else if (" << stmt.expr->translateToCpp() << ")" << std::endl;
            oss << stmt.body->translateToCpp();
        }
        else
        {
            oss << "else " << std::endl;
            oss << stmt.body->translateToCpp();
        }
    }

    return oss.str();
}