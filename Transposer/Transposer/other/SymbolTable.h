#pragma once
#include <map>
#include <set>

#include "Scope.h"
#include "class/ClassNode.h"
#include "class/ClassTree.h"
#include "symbols/Func.h"
#include "symbols/FuncCredit.h"

class IFuncDeclStmt;
class FuncCallExpr;

class SymbolTable
{
private:
    std::unique_ptr<Scope> head;
    Scope* currScope;
    IFuncDeclStmt* currFunc;
    std::set<std::pair<Func, bool>> funcs; // the bool means is it included to this file
    ClassNode* currClass = nullptr;

    static ClassTree& classTree;

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
    void addFunc(const Func& f, const bool isIncluded = false);

    void enterScope(bool isBreakable, bool isContinueAble);
    void exitScope();

    int getLevel() const;

    void changeFunc(IFuncDeclStmt* funcDecl);

    Scope* getCurrScope() const;
    IFuncDeclStmt* getCurrFunc() const;
    std::unique_ptr<Func> getFunc(const std::wstring& name) const;

    std::string getFuncsHeaders() const;

    void addClass(const Class& cls, ClassNode* parent = nullptr);
    void resetCurrClass();
    const ClassNode* getCurrClass() const;

    bool addField(const AccessType accessType, const Var& field, const Token& token) const;
    bool addMethod(const AccessType accessType, const Func& method, const Token& token) const;
    bool addCtor(const AccessType accessType, const Constractor& ctor, const Token& token) const;

    SymbolTable& operator+=(const SymbolTable& other);

    static void clearClasses();
    static bool isClass(const std::wstring& name);
    static ClassNode* getClass(const std::wstring& name);
    static bool isLegalFieldOrMethod(const std::unique_ptr<IType>& type, const std::wstring& name, const Token& token, const ClassNode* curr);
};
