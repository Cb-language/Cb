#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../../../symbols/Type/Type.h"
#include "AST/abstract/Call.h"

class FuncCallExpr : public Call
{
private:
    const std::wstring name;
    std::vector<std::unique_ptr<Expr>> args;
    std::unique_ptr<IType> type;
    const bool isStmt;

public:
    FuncCallExpr(Scope* scope, FuncDeclStmt* funcDecl, const std::wstring& name, std::vector<std::unique_ptr<Expr>> args, const bool isStmt);

    std::unique_ptr<IType> getType() const override;
    bool isLegal() const override;
    std::string translateToCpp() const override;

    void setType(std::unique_ptr<IType> type);
    bool isLegalCall(const Func& func) const;
};
