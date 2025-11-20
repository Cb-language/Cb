#pragma once

#include <memory>
#include <optional>

#include "../Statements.h"

class VarDecStmt : public Stmt
{
private:
    const bool hasStartingValue;
    const Type type;
    const std::unique_ptr<Expr> startingValue;

public:
    VarDecStmt(const bool hasStartingValue, const Type type, std::unique_ptr<Expr> startingValue);

    bool isLegal() const override;
    std::string translateToCpp() const override;

    Type Gettype() const;

};