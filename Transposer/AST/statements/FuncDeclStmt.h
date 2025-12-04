#pragma once

#include "../Statements.h"

class FuncDeclStmt : public Stmt
{
private:
    std::vector<std::unique_ptr<Var>> params;
    std::string funcName;
    Type returnType;
public:
    FuncDeclStmt(Scope* scope, const std::string &funcName, const Type &returnType, std::vector<std::unique_ptr<Var>> &&params);

    const std::vector<std::unique_ptr<Var>>& getParams() const;
    std::string getName() const;
    Type getReturnType() const;

    bool isLegal() const override;
    std::string translateToCpp() const override;
};