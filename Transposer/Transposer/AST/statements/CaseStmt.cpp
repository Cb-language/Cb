#include "CaseStmt.h"

#include "errorHandling/how/HowDidYouGetHere.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"

CaseStmt::CaseStmt(const Token& token, Scope *scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass, std::unique_ptr<Expr> expr, std::unique_ptr<BodyStmt> body,
                   const bool isDefault) : Stmt(token, scope, funcDecl, currClass), expr(std::move(expr)), body(std::move(body)), isDefault(isDefault)
{
}

void CaseStmt::analyze() const
{
    // Can't really get here, but it's good to check
    if (body == nullptr)
    {
        throw HowDidYouGetHere(token);
    }

    if (!expr->getType()->isNumberable())
    {
        throw IllegalTypeCast(token, expr->getType()->toString(), "degree");
    }

    body->analyze();
    expr->analyze();
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
        os << "case " << expr->translateToCpp();
    }
    os << ":\n" << body->translateToCpp() << "\n";
    return os.str();
}
