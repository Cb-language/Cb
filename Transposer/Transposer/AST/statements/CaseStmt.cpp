#include "CaseStmt.h"

#include "errorHandling/how/HowDidYouGetHere.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"

CaseStmt::CaseStmt(const Token& token, StmtWithBody stmt,
                   const bool isDefault) : Stmt(token), stmt(std::move(stmt)), isDefault(isDefault)
{
}

void CaseStmt::analyze() const
{
    // Can't really get here, but it's good to check
    if (stmt.body == nullptr)
    {
        throw HowDidYouGetHere(token);
    }

    if (!stmt.expr->getType()->isNumberable())
    {
        throw IllegalTypeCast(token, stmt.expr->getType()->toString(), "degree");
    }

    stmt.body->analyze();
    stmt.expr->analyze();
}

std::string CaseStmt::translateToCpp() const
{
    std::ostringstream os;
    os << getTabs();

    if (isDefault)
    {
        os << "default";
    }
    else
    {
        os << "case " << stmt.expr->translateToCpp();
    }
    os << ":\n" << stmt.body->translateToCpp() << "\n";
    return os.str();
}
