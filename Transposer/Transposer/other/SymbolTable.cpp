#include "SymbolTable.h"

#include <ranges>
#include <sstream>

#include "AST/statements/expression/FuncCallExpr.h"

SymbolTable::SymbolTable()
{
    head = std::make_unique<Scope>(nullptr, false);
    currScope = head.get();
    currFunc = nullptr; // global is null
}

SymbolTable::~SymbolTable()
{
    funcs.clear();
    head.reset();
    head = nullptr;
    currScope = nullptr;
    currClass = nullptr;
}

std::optional<Var> SymbolTable::getVar(const std::wstring &name) const
{
    return currScope->getVar(name, currClass);
}

void SymbolTable::addVar(std::unique_ptr<IType> type, const Token& token) const
{
    currScope->addVar(std::move(type), token);
}

void SymbolTable::addVar(const Var& var, const Token& token) const
{
    currScope->addVar(var, token);
}

std::optional<Class> SymbolTable::getClass(const std::wstring& name)
{
    // TODO: replace it with tree search

    for (const auto* c : classes)
    {
        if (c->getClass().getClassName() == name) return c->getClass().copy();
    }

    return std::nullopt;
}

bool SymbolTable::isClass(const std::wstring& name)
{
     return getClass(name) != std::nullopt;
}

bool SymbolTable::doesFuncExist(const Func& f) const
{
    for (const auto& func : funcs | std::views::keys)
    {
        if (func == f)
        {
            return true;
        }
    }

    return false;
}

bool SymbolTable::doesFuncExist(const std::wstring& name) const
{
    for (const auto& func : funcs| std::views::keys)
    {
        if (func.getFuncName() == name)
        {
            return true;
        }
    }
    return false;
}

bool SymbolTable::isLegalCredit(const FuncCredit& credit) const
{
    for (const auto& func : funcs| std::views::keys)
    {
        if (credit.isLegalCredit(func))
        {
            return true;
        }
    }

    return false;
}

std::unique_ptr<IType> SymbolTable::getCallType(FuncCallExpr* expr) const
{
    for (const auto& func : funcs| std::views::keys)
    {
        if (expr->isLegalCall(func))
        {
            return func.getType()->copy();
        }
    }

    return nullptr;
}

void SymbolTable::addFunc(const Func& f, const bool isIncluded)
{
    funcs.insert(std::make_pair(f.copy(), isIncluded));
}

void SymbolTable::enterScope(const bool isBreakable, const bool isContinueAble)
{
    currScope = currScope->makeNewScope(isBreakable, isContinueAble);
}

void SymbolTable::exitScope()
{
    currScope = currScope->getParent();
}

int SymbolTable::getLevel() const
{
    return currScope->getLevel();
}

void SymbolTable::changeFunc(IFuncDeclStmt* funcDecl)
{
    currFunc = funcDecl;
}

Scope* SymbolTable::getCurrScope() const
{
    return currScope;
}

IFuncDeclStmt* SymbolTable::getCurrFunc() const
{
    return currFunc;
}

std::unique_ptr<Func> SymbolTable::getFunc(const std::wstring& name) const
{
    if (!doesFuncExist(name))
    {
        return nullptr;
    }
    for (const auto& func : funcs | std::views::keys)
    {
        if (func.getFuncName() == name)
        {
            return std::make_unique<Func>(func.copy());
        }
    }
    return nullptr;
}

std::string SymbolTable::getFuncsHeaders() const
{
    std::ostringstream oss;
    for (const auto& [func, isIncluded] : funcs)
    {
        // convention to note write the main's header
        if (func.getFuncName() != L"prelude" && !isIncluded)
        {
            oss << func.translateToCpp() << ";" << std::endl;
        }
    }

    return oss.str();
}

void SymbolTable::setClass(const Class& cls)
{
    currClass = new ClassNode(cls);
    classes.emplace_back(currClass);
}

const ClassNode* SymbolTable::getCurrClass() const
{
    return currClass;
}

SymbolTable& SymbolTable::operator+=(const SymbolTable& other)
{
    for (const auto& func : other.funcs | std::views::keys)
    {
        addFunc(func, true);
    }

    if (other.head == nullptr) return *this;

    for (const auto& [var, token] : other.head->getCurrVars())
    {
        addVar(var.copy(), token);
    }

    return *this;
}

void SymbolTable::clearClasses()
{
    for (const auto* cls : classes) delete cls;
    classes.clear();
}
