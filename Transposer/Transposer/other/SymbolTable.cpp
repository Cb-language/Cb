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
    errors.clear();
}

void SymbolTable::analyzePass1(const std::vector<std::unique_ptr<Stmt>>& stmts)
{
    // Pass 1: Global Declarations (Names only)
    // Register all classes and global function signatures so they can be referenced by name.
    for (const auto& stmt : stmts)
    {
        if (const auto* classDecl = dynamic_cast<ClassDeclStmt*>(stmt.get()))
        {
            ClassNode* parentNode = nullptr;
            if (const FQN& parentName = classDecl->getParentName(); !parentName.empty()) parentNode = classTree.find(parentName);
            addClass(Class(classDecl->getName()), parentNode);
        }
        else if (auto* funcDecl = dynamic_cast<FuncDeclStmt*>(stmt.get()))
        {
            Func f = funcDecl->getFunc();
            funcs[f] = std::make_pair(false, funcDecl);

            // Prelude entry-point validation
            if (translateFQNtoString(funcDecl->getName()) == "prelude")
            {
                if (hasMainFound) addError(std::make_unique<MainOverride>(funcDecl->getToken()));

                if (funcDecl->getReturnType()->toString() != "degree")
                {
                    addError(std::make_unique<InvalidMainReturnType>(funcDecl->getToken()));
                }

                if (!funcDecl->getArgs().empty())
                {
                    addError(std::make_unique<InvalidMainArgs>(funcDecl->getToken()));
                }

                hasMainFound = true;
                mainPath = funcDecl->getToken().path;
            }
        }
    }
    resetCurrClass();
}

void SymbolTable::analyzePass2(const std::vector<std::unique_ptr<Stmt>>& stmts)
{
    // Pass 2: Structure and Member Signatures
    // Resolve class hierarchy and register all fields, methods, and constructors.
    // This allows any body stmt to refer to any member of any class.
    for (const auto& stmt : stmts)
    {
        stmt->setSymbolTable(this);
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
                    field->setSymbolTable(this);
                    field->setScope(currScope);
                    field->setClassNode(node);
                    addField(access, field->getVar(), field->getToken());
                }
                for (const auto& [access, method] : classDecl->getMethods())
                {
                    method->setSymbolTable(this);
                    method->setScope(currScope);
                    method->setClassNode(node);
                    Func f = method->getFunc();
                    f.setOwner(node);
                    addMethod(access, f, method->getToken());
                    funcs[f] = std::make_pair(false, method.get());
                }
                for (const auto& [access, ctor] : classDecl->getCtors())
                {
                    ctor->setSymbolTable(this);
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
}

void SymbolTable::analyzePass3(const std::vector<std::unique_ptr<Stmt>>& stmts)
{
    // Pass 3: Full Semantic Analysis (Bodies and Initializers)
    // Now that all signatures are known, perform deep analysis.
    for (const auto& stmt : stmts)
    {
        // Ensure the top-level statement has the correct context
        stmt->setSymbolTable(this);
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
        try {
            stmt->analyze();
        } catch (const Error& e) {
            addError(std::unique_ptr<Error>(e.copy()));
        }
    }
}

void SymbolTable::addError(std::unique_ptr<Error> err) const
{
    errors.push_back(std::move(err));
}

std::vector<Error*> SymbolTable::getErrors() const
{
    std::vector<Error*> errs;
    for (const auto& err : errors)
    {
        errs.push_back(err.get());
    }
    return errs;
}

std::optional<Var> SymbolTable::getVar(const FQN &name, const ClassNode* contextClass) const
{
    if (name.empty()) return std::nullopt;

    // Use contextClass if provided, otherwise fallback to symTable's currClass (if any)
    const ClassNode* currentContext = contextClass != nullptr ? contextClass : currClass;

    // 1. Resolve the first part of the FQN
    const FQN first = {name[0]};
    std::optional<Var> currentVar;

    if (currentContext != nullptr)
    {
        if (const auto varPtr = currentContext->findField(first))
        {
            currentVar = varPtr->copy();
        }
    }
    
    if (!currentVar)
    {
        currentVar = currScope->getVar(first, currentContext);
    }

    if (!currentVar) return std::nullopt;

    // 2. Resolve remaining parts (member access)
    for (size_t i = 1; i < name.size(); ++i)
    {
        auto type = currentVar->getType();
        const auto* classType = dynamic_cast<ClassType*>(type.get());
        if (!classType) return std::nullopt; // Not a class object, can't have fields

        const ClassNode* node = classTree.find(classType->getFQN());
        if (!node) return std::nullopt;

        const Var* field = node->findField({name[i]}, currentContext);
        if (!field) return std::nullopt;

        currentVar = field->copy();
    }

    return currentVar;
}

void SymbolTable::addVar(std::unique_ptr<IType> type, const Token& token) const
{
    currScope->addVar(this, std::move(type), token);
}

void SymbolTable::addVar(const Var& var, const Token& token) const
{
    currScope->addVar(this, var, token);
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
    return funcs.contains(f);
}

bool SymbolTable::doesFuncExist(const FQN& name, const ClassNode* owner) const
{
    for (const auto& func : funcs | std::views::keys)
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
    for (const auto& func : funcs | std::views::keys)
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
    // Check for constructor call (the name is a class name)
    if (const ClassNode* classNode = classTree.find(expr->getName()))
    {
        for (const auto& [accessType, ctor] : classNode->getClass().getConstractors())
        {
            if (expr->argsMatch(ctor))
            {
                const_cast<FuncCallExpr*>(expr)->setTargetClass(classNode);
                return std::make_unique<ClassType>(classNode->getClass().getClassName());
            }
        }
    }

    if (const FQN& fqn = expr->getName(); fqn.size() > 1)
    {
        // Member access: rex.speak() or Animal.kingdom()
        FQN targetName = fqn;
        const std::string methodName = targetName.back();
        targetName.pop_back();

        const ClassNode* targetClassNode = nullptr;

        // 1. Try to resolve targetName as a variable (e.g., 'rex')
        if (const auto var = getVar(targetName, callClass))
        {
            const auto type = var->getType();
            if (const auto* classType = dynamic_cast<ClassType*>(type.get()))
            {
                targetClassNode = classTree.find(classType->getFQN());
            }
        }

        // 2. Try to resolve targetName as a class name (static call, e.g., 'Animal')
        if (targetClassNode == nullptr)
        {
            targetClassNode = classTree.find(targetName);
        }

        if (targetClassNode != nullptr)
        {
            const ClassNode* current = targetClassNode;
            while (current != nullptr)
            {
                for (const auto& [func, info] : funcs)
                {
                    if (func.getOwner() == current && 
                        func.getFuncName().size() == 1 &&
                        func.getFuncName()[0] == methodName && 
                        expr->argsMatch(func))
                    {
                        // Check access rights
                        if (current->isLegal(func, callClass))
                        {
                            if (info.second != nullptr) const_cast<FuncCallExpr*>(expr)->setClassDecl(*(info.second));
                            const_cast<FuncCallExpr*>(expr)->setTargetClass(current);
                            return func.getType();
                        }
                    }
                }
                current = current->getParent();
            }
        }
    }
    else
    {
        // Local or Global call
        const ClassNode* current = callClass;
        while (current != nullptr)
        {
            for (const auto& [func, info] : funcs)
            {
                if (func.getOwner() == current && expr->isLegalCall(func))
                {
                    // check access
                    if (current->isLegal(func, callClass))
                    {
                        if (info.second != nullptr) const_cast<FuncCallExpr*>(expr)->setClassDecl(*(info.second));
                        return func.getType();
                    }
                }
            }
            current = current->getParent();
        }

        for (const auto& [func, info] : funcs)
        {
            if (func.getOwner() == nullptr && expr->isLegalCall(func))
            {
                if (info.second != nullptr) const_cast<FuncCallExpr*>(expr)->setClassDecl(*(info.second));
                return func.getType()->copy();
            }
        }
    }

    return nullptr;
}

void SymbolTable::addFunc(const Func& f, const bool isIncluded)
{
    funcs[f] = std::make_pair(isIncluded, nullptr);
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
    for (const auto& [func, info] : funcs)
    {
        // convention to note write the main's header
        if (translateFQNtoString(func.getFuncName()) != "prelude" && !info.first)
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

std::optional<std::filesystem::path> SymbolTable::getMainPath() const
{
    return hasMainFound ? std::make_optional(mainPath) : std::nullopt;
}

void SymbolTable::clearClasses()
{
    ClassTree::destroy();
}

bool SymbolTable::isLegalFieldOrMethod(const SymbolTable* symTable, const std::unique_ptr<IType>& type, const FQN& name, const Token& token, const ClassNode* currClass)
{
    const ClassNode* res = classTree.find(name);

    if (res == nullptr)
    {
        if (symTable) symTable->addError(std::make_unique<HowDidYouGetHere>(token));
        return false;
    }

    if (const Var* field = res->findField(name, currClass); field != nullptr)
    {
        if (res->isLegal(*field, currClass)) return true;

        if (symTable) symTable->addError(std::make_unique<AccessError>(token, translateFQNtoString(res->getClass().getClassName()), translateFQNtoString(name)));
        return false;
    }

    if (const Func* method = res->findMethod(name, currClass); method != nullptr)
    {
        if (res->isLegal(*method, currClass)) return true;

        if (symTable) symTable->addError(std::make_unique<AccessError>(token, translateFQNtoString(res->getClass().getClassName()), translateFQNtoString(name)));
        return false;
    }

    if (currClass != nullptr)
    {
        if (const auto parent = currClass->getParent(); parent != nullptr)
        {
            return isLegalFieldOrMethod(symTable, std::make_unique<ClassType>(name), name, token, parent);
        }
    }

    return false;
}
