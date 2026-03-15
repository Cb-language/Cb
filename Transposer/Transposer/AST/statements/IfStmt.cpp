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
    if (symTable == nullptr) return;

    ifStmt.expr->setSymbolTable(symTable);
    ifStmt.expr->setScope(scope);
    ifStmt.expr->setClassNode(currClass);
    ifStmt.expr->analyze();

    ifStmt.body->setSymbolTable(symTable);
    ifStmt.body->setScope(scope);
    ifStmt.body->setClassNode(currClass);
    ifStmt.body->analyze();

    for (const auto& stmt : elseStmts)
    {
        if (stmt.expr != nullptr)
        {
            stmt.expr->setSymbolTable(symTable);
            stmt.expr->setScope(scope);
            stmt.expr->setClassNode(currClass);
            stmt.expr->analyze();
        }

        stmt.body->setSymbolTable(symTable);
        stmt.body->setScope(scope);
        stmt.body->setClassNode(currClass);
        stmt.body->analyze();
    }
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

void IfStmt::setSymbolTable(SymbolTable* symTable) const
{
    Stmt::setSymbolTable(symTable);
    ifStmt.expr->setSymbolTable(symTable);
    ifStmt.body->setSymbolTable(symTable);

    for (auto& stmt : elseStmts)
    {
        stmt.body->setSymbolTable(symTable);

        if (stmt.expr != nullptr) // if its an else stmt there's no else
            stmt.expr->setSymbolTable(symTable);
    }
}
