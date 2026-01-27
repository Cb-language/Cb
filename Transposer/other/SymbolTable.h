#pragma once
#include <stack>

#include "Scope.h"
#include "symbols/Func.h"
#include "symbols/FuncCredit.h"

class FuncDeclStmt;
class FuncCallExpr;

class SymbolTable
{
private:
    std::unique_ptr<Scope> head;
    Scope* currScope;
    FuncDeclStmt* currFunc;
    std::vector<Func> funcs;

public:
    SymbolTable();
    ~SymbolTable();

    // std::nullopt when not found
    std::optional<Var> getVar(const std::wstring& name) const;
    void addVar(std::unique_ptr<IType> type, const Token& token) const;
    void addVar(const Var& var, const Token& token) const;

    bool doesFuncExist(const Func& f) const;
    bool doesFuncExist(const std::wstring& name) const;
    bool isLegalCredit(const FuncCredit& credit) const;
    std::unique_ptr<IType> getCallType(FuncCallExpr* expr) const;
    void addFunc(const Func& f);

    void enterScope(bool isBreakable, bool isContinueAble);
    void exitScope();

    int getLevel() const;

    void changeFunc(FuncDeclStmt* funcDecl);

    Scope* getCurrScope() const;
    FuncDeclStmt* getCurrFunc() const;
    std::unique_ptr<Func> getFunc(const std::wstring& name) const;

    std::string getFuncsHeaders() const;

    SymbolTable& operator+=(const SymbolTable& other);
};
