#include "HearStmt.h"

#include <sstream>

#include "errorHandling/semanticErrors/IllegalHear.h"
#include "other/SymbolTable.h"

HearStmt::HearStmt(const Token& token, std::vector<std::unique_ptr<VarReference>>& refs)
    : Stmt(token)
{
    for (auto& ref : refs)
    {
        this->references.push_back(std::move(ref));
    }
}

void HearStmt::analyze() const
{
    if (symTable == nullptr) return;

    for (const auto& ref : references)
    {
        ref->setSymbolTable(symTable);
        ref->setScope(scope);
        ref->setClassNode(currClass);
        ref->analyze();

        if (ref->getType()->getArrLevel() > 0 || !(ref->getType()->isPrimitive()))
        {
            symTable->addError(std::make_unique<IllegalHear>(token, ref->getType()->toString()));
        }
    }
}

std::string HearStmt::translateToCpp() const
{
    std::ostringstream oss;

    if (references.empty())
    {
        oss << getTabs() << "system(\"pause\");";
        return oss.str();
    }

    oss << getTabs() << "std::cin";

    for (const auto& ref : references)
    {
        oss << " >> " << Utils::removeAllFirstTabs(ref->translateToCpp());
    }

    oss << ";";

    return oss.str();
}

void HearStmt::setSymbolTable(SymbolTable* symTable) const
{
    Stmt::setSymbolTable(symTable);
    for (const auto& ref : references) ref->setSymbolTable(symTable);
}
