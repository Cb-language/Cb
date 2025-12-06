#pragma once

#include "BodyStmt.h"
#include "AST/abstract/Statement.h"

class BodyStmt;

class FuncDeclStmt : public Stmt
{
private:
    Func func;
    std::vector<std::unique_ptr<Func>> credited;
    std::unique_ptr<BodyStmt> body;
    bool hasReturned;

public:
    FuncDeclStmt(Scope* scope, const std::wstring &funcName, const Type &returnType, const std::vector<Var> &args, std::vector<std::unique_ptr<Func>>& credited);

    const std::vector<Var>& getArgs() const;
    std::wstring getName() const;
    Type getReturnType() const;
    const Func& getFunc() const;

    // is called after the ctor
    void setBody(std::unique_ptr<BodyStmt> body);

    void setHasReturned(const bool hasReturned);
    bool getHasReturned() const;

    bool isLegal() const override;
    std::string translateToCpp() const override;
};
