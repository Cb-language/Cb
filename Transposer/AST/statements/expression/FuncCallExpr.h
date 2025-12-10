#pragma once

#include <string>
#include <vector>
#include <memory>

#include "symbols/Type.h"
#include "AST/abstract/Expression.h"

class FuncCallExpr : public Expr
{
private:
    const std::wstring name;
    std::vector<std::unique_ptr<Expr>> args;
    Type type;
    const bool isStmt;

public:
    FuncCallExpr(Scope* scope, FuncDeclStmt* funcDecl,  const std::wstring& name, std::vector<std::unique_ptr<Expr>> args, const bool isStmt);

    Type getType() const override;
    bool isLegal() const override;
    std::string translateToCpp() const override;

    void setType(Type type);
    bool isLegalCall(const Func& func) const;
};
