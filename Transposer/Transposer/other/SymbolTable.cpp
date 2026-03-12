// ReSharper disable CppExpressionWithoutSideEffects
#include "SymbolTable.h"

#include <ranges>
#include <sstream>

#include "AST/statements/expression/FuncCallExpr.h"
#include "AST/statements/ClassDeclStmt.h"
#include "AST/statements/FuncDeclStmt.h"
#include "AST/statements/VarDeclStmt.h"
#include "AST/statements/ConstructorDeclStmt.h"
#include "errorHandling/classErrors/AccessError.h"
#include "errorHandling/entryPointErrors/InvalidMainArgs.h"
#include "errorHandling/entryPointErrors/InvalidMainReturnType.h"
#include "errorHandling/entryPointErrors/MainOverride.h"
#include "errorHandling/semanticErrors/IdentifierTaken.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "symbols/Type/ClassType.h"

ClassTree& SymbolTable::classTree = ClassTree::instance();

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

void SymbolTable::analyze(const std::vector<std::unique_ptr<Stmt>>& stmts)
{
    // Pass 1: Global Declarations (Names only)
    // Register all classes and global function signatures so they can be referenced by name.
    for (const auto& stmt : stmts)
    {
        if (const auto* classDecl = dynamic_cast<ClassDeclStmt*>(stmt.get()))
        {
            addClass(Class(classDecl->getName()));
        }
        else if (const auto* funcDecl = dynamic_cast<FuncDeclStmt*>(stmt.get()))
        {
            addFunc(funcDecl->getFunc());

            // Prelude entry-point validation
            if (translateFQNtoString(funcDecl->getName()) == "prelude")
            {
                if (hasMainFound) throw MainOverride(funcDecl->getToken());
                hasMainFound = true;

                if (funcDecl->getReturnType()->toString() != "degree")
                {
                    throw InvalidMainReturnType(funcDecl->getToken());
                }

                if (!funcDecl->getArgs().empty())
                {
                    throw InvalidMainArgs(funcDecl->getToken());
                }
            }
        }
    }
    resetCurrClass();

    // Pass 2: Structure and Member Signatures
    // Resolve class hierarchy and register all fields, methods, and constructors.
    // This allows any body to refer to any member of any class.
    for (const auto& stmt : stmts)
    {
        if (const auto* classDecl = dynamic_cast<ClassDeclStmt*>(stmt.get()))
        {
            if (ClassNode* node = classTree.find(classDecl->getName()))
            {
                // Resolve and set parent
                if (const FQN& parentName = classDecl->getParentName(); !parentName.empty())
                {
                    if (ClassNode* parentPtr = classTree.find(parentName)) node->setParent(parentPtr);
                }

                // Add inner members (fields, methods, ctors) to the class node
                currClass = node;
                for (const auto& [access, field] : classDecl->getFields())
                {
                    field->setScope(currScope);
                    field->setClassNode(node);
                    addField(access, field->getVar(), field->getToken());
                }
                for (const auto& [access, method] : classDecl->getMethods())
                {
                    method->setScope(currScope);
                    method->setClassNode(node);
                    Func f = method->getFunc();
                    f.setOwner(node);
                    addMethod(access, f, method->getToken());
                    addFunc(f); // Make methods available for lookup via call resolution
                }
                for (const auto& [access, ctor] : classDecl->getCtors())
                {
                    ctor->setScope(currScope);
                    ctor->setClassNode(node);
                    addCtor(access, ctor->getConstractor(), ctor->getToken());
                }
                resetCurrClass();
            }
        }
        else if (auto* varDecl = dynamic_cast<VarDeclStmt*>(stmt.get()))
        {
            // Register global variables so they are available in Pass 3 initializers
            varDecl->setScope(currScope);
            varDecl->setClassNode(nullptr);
            addVar(varDecl->getVar(), varDecl->getToken());
        }
    }

    // Pass 3: Full Semantic Analysis (Bodies and Initializers)
    // Now that all signatures are known, perform deep analysis.
    for (const auto& stmt : stmts)
    {
        // Ensure the top-level statement has the correct context
        stmt->setScope(currScope);
        if (const auto* classDecl = dynamic_cast<ClassDeclStmt*>(stmt.get()))
        {
            stmt->setClassNode(classTree.find(classDecl->getName()));
        }
        else
        {
            stmt->setClassNode(nullptr);
        }

        // Recursively analyze the statement (will handle nested bodies and expressions)
        stmt->analyze();
    }
}

std::optional<Var> SymbolTable::getVar(const FQN &name) const
{
    if (currClass != nullptr)
    {
        if (const auto varPtr = currClass->findField(name))
        {
            return varPtr->copy();
        }
    }
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

ClassNode* SymbolTable::getClass(const FQN& name)
{
    return classTree.find(name);
}

bool SymbolTable::isClass(const FQN& name)
{
     return getClass(name) != nullptr;
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

bool SymbolTable::doesFuncExist(const FQN& name, const ClassNode* owner) const
{
    for (const auto& func : funcs| std::views::keys)
    {
        if (func.getFuncName() == name && func.getOwner() == owner)
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

std::unique_ptr<IType> SymbolTable::getCallType(const FuncCallExpr* expr, const ClassNode* callClass) const
{
    const ClassNode* current = callClass;
    while (current != nullptr)
    {
        for (const auto& func : funcs | std::views::keys)
        {
            if (func.getOwner() == current && expr->isLegalCall(func))
            {
                // check access
                if (current->isLegal(func, callClass))
                {
                    return func.getType();
                }
            }
        }
        current = current->getParent();
    }

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

std::unique_ptr<Func> SymbolTable::getFunc(const FQN& name, const ClassNode* owner) const
{
    if (!doesFuncExist(name, owner))
    {
        return nullptr;
    }
    for (const auto& func : funcs | std::views::keys)
    {
        if (func.getFuncName() == name && func.getOwner() == owner)
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
        if (translateFQNtoString(func.getFuncName()) != "prelude" && !isIncluded)
        {
            oss << func.translateToCpp() << ";" << std::endl;
        }
    }

    return oss.str();
}

void SymbolTable::addClass(const Class& cls, ClassNode* parent)
{
    classTree.addClass(cls, parent);
    currClass = classTree.find(cls.getClassName());
}

void SymbolTable::resetCurrClass()
{
    currClass = nullptr;
}

const ClassNode* SymbolTable::getCurrClass() const
{
    return currClass;
}

bool SymbolTable::addField(const AccessType accessType, const Var& field, const Token& token) const
{
    if (currClass == nullptr) return false;

    currClass->add(accessType, field);
    return true;
}

bool SymbolTable::addMethod(const AccessType accessType, const Func& method, const Token& token) const
{
    if (currClass == nullptr) return false;

    currClass->add(accessType, method);
    return true;
}

bool SymbolTable::addCtor(const AccessType accessType, const Constractor& ctor, const Token& token) const
{
    if (currClass == nullptr) return false;

    currClass->add(accessType, ctor);
    return true;
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
    ClassTree::destroy();
}

bool SymbolTable::isLegalFieldOrMethod(const std::unique_ptr<IType>& type, const FQN& name, const Token& token, const ClassNode* currClass)
{
    const ClassNode* res = classTree.find(name);

    if (res == nullptr) throw HowDidYouGetHere(token);

    if (const Var* field = res->findField(name, currClass); field != nullptr)
    {
        if (res->isLegal(*field, currClass)) return true;

        throw AccessError(token, translateFQNtoString(res->getClass().getClassName()), translateFQNtoString(name));
    }

    if (const Func* method = res->findMethod(name, currClass); method != nullptr)
    {
        if (res->isLegal(*method, currClass)) return true;

        throw AccessError(token, translateFQNtoString(res->getClass().getClassName()), translateFQNtoString(name));
    }

    if (currClass != nullptr)
    {
        if (auto parent = currClass->getParent())
        {
            return isLegalFieldOrMethod(std::make_unique<ClassType>(name), name, token, currClass->getParent());
        }
    }

    return false;
}
