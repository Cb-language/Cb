#pragma once

#include "../Statements.h"

class FuncDecStmt : public Stmt
{
private:
    std::vector<std::unique_ptr<Var>> params;
    std::string funcName;
    Type returnType;
public:
    FuncDecStmt(Scope* scope, FuncDeclStmt* funcDecl, const std::string &funcName, const Type &returnType, std::vector<std::unique_ptr<Var>> &&params);

    std::vector<std::unique_ptr<Var>> getParams() const;
    std::string getName() const;
    Type getReturnType() const;

    bool isLegal() const override;
    std::string translateToCpp() const override;
};