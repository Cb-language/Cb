#include "FuncDeclStmt.h"

#include "errorHandling/semanticErrors/NoReturn.h"
#include "../../errorHandling/classErrors/VirtualNonMethod.h"
#include "errorHandling/classErrors/PureWithBody.h"
#include "other/SymbolTable.h"

FuncDeclStmt::FuncDeclStmt(const Token& token,  const FQN& funcName, std::unique_ptr<IType> returnType, const std::vector<Var>& args,
    std::vector<std::unique_ptr<FuncCreditStmt>>& credited, const bool isMethod, const VirtualType& virtualType, const bool isStatic)
        : IFuncDeclStmt(token), func(Func(std::move(returnType), funcName, args, virtualType, nullptr, isStatic)), body(nullptr), hasReturned(false), virtualType(virtualType), isMethod(isMethod)
{
    for (auto& c : credited)
    {
        this->credited.emplace_back(std::move(c));
    }
}

const std::vector<Var>& FuncDeclStmt::getArgs() const
{
    return func.getArgs();
}

const FQN& FuncDeclStmt::getName() const
{
    return func.getFuncName();
}

std::unique_ptr<IType> FuncDeclStmt::getReturnType() const
{
    return func.getType();
}

Func FuncDeclStmt::getFunc() const
{
    return func;
}

void FuncDeclStmt::setVirtual(const VirtualType vType)
{
    this->virtualType = vType;
    this->func.setVirtual(vType);
}

VirtualType FuncDeclStmt::getVirtual() const
{
    return virtualType;
}

void FuncDeclStmt::setIsStatic(const bool isStatic)
{
    func.setStatic(isStatic);
}

void FuncDeclStmt::setBody(std::unique_ptr<BodyStmt> body)
{
    this->body = std::move(body);
}

void FuncDeclStmt::setHasReturned(const bool hasReturned)
{
    this->hasReturned = hasReturned;
}

bool FuncDeclStmt::getHasReturned() const
{
    return hasReturned;
}

bool FuncDeclStmt::getIsMethod() const
{
    return isMethod;
}

const std::vector<std::unique_ptr<FuncCreditStmt>>& FuncDeclStmt::getCredited() const
{
    return credited;
}

void FuncDeclStmt::analyze() const
{
    if (symTable == nullptr) return;

    if (virtualType != VirtualType::NONE && !isMethod)
    {
        symTable->addError(std::make_unique<VirtualNonMethod>(token));
    }

    if (virtualType != VirtualType::PURE)
    {
        symTable->changeFunc(const_cast<FuncDeclStmt*>(this));
        
        symTable->enterScope(false, false);
        for (const auto& arg : func.getArgs())
        {
            symTable->addVar(arg, token);
        }
        
        body->setSymbolTable(symTable);
        body->setScope(symTable->getCurrScope());
        body->setClassNode(symTable->getCurrClass());

        for (const auto& stmt : body->getStmts())
        {
            stmt->setSymbolTable(symTable);
            stmt->setScope(symTable->getCurrScope());
            stmt->setClassNode(symTable->getCurrClass());
            stmt->analyze();
        }
        
        symTable->exitScope();

        if (func.getType()->toString() != "fermata" && !hasReturned)
        {
            symTable->addError(std::make_unique<NoReturn>(token));
        }
    }
    else
    {
        if (body != nullptr)
        {
            symTable->addError(std::make_unique<PureWithBody>(token));
        }
    }
}


std::string FuncDeclStmt::translateToCpp() const
{
    std::ostringstream oss;
    oss << func.translateToCpp();

    oss << std::endl << "{" << std::endl;
    for (const auto& s : body->getStmts()) oss << s->translateToCpp() << std::endl;
    oss << "}" << std::endl;

    return oss.str();
}

std::string FuncDeclStmt::translateToH() const
{
    if (translateFQNtoString(func.getFuncName()) == "prelude") return "";

    const std::string fStr = func.translateToCpp();
    std::string prefix = "";
    if (func.getStatic()) prefix = "static ";

    switch (virtualType)
    {
        case VirtualType::PURE: return "virtual " + fStr + " = 0;";
        case VirtualType::VIRTUAL: return "virtual " + fStr + ";";
        case VirtualType::OVERRIDE: return fStr + " override;";
        default: return prefix + fStr + ";";
    }
}

std::string FuncDeclStmt::translateToCppClass(const std::string& className) const
{
    if (virtualType != VirtualType::PURE)
    {
        std::ostringstream oss;
        oss << func.translateToCpp(className);

        oss << std::endl << "{" << std::endl;
        for (const auto& s : body->getStmts()) oss << s->translateToCpp() << std::endl;
        oss << "}" << std::endl;

        return oss.str();
    }

    return "";
}

void FuncDeclStmt::setSymbolTable(SymbolTable* symTable) const
{
    Stmt::setSymbolTable(symTable);

    if (virtualType != VirtualType::PURE)
    {
        body->setSymbolTable(symTable);
    }
}
