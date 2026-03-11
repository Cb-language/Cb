#pragma once
#include "AST/abstract/Statement.h"

class ContinueStmt : public Stmt
{
private:

public:
    explicit ContinueStmt(const Token& token);
    void analyze() const override;
    std::string translateToCpp() const override;
};
