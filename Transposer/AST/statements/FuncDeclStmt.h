#pragma once

#include "AST/abstract/Statement.h"

class FuncDeclStmt : public Stmt
{
private:
    Func func;
    std::vector<std::unique_ptr<Func>> credited;
public:
    FuncDeclStmt(Scope* scope, const std::wstring &funcName, const Type &returnType, const std::vector<Var> &args, std::vector<std::unique_ptr<Func>>& credited);

    const std::vector<Var>& getArgs() const;
    std::wstring getName() const;
    Type getReturnType() const;

    bool isLegal() const override;
    std::string translateToCpp() const override;
};
