#include "FuncDecStmt.h"

FuncDecStmt::FuncDecStmt(Scope *scope, FuncDeclStmt *funcDecl, const std::string &funcName,
    const Type &returnType, std::vector<std::unique_ptr<Var>> &&params) : Stmt(scope, funcDecl), funcName(funcName), returnType(returnType)
{
    for (auto& param : params)
    {
        this->params.emplace_back(std::move(param));
    }
}

std::vector<std::unique_ptr<Var>> FuncDecStmt::getParams() const
{
    return params;
}

std::string FuncDecStmt::getName() const
{
    return funcName;
}

Type FuncDecStmt::getReturnType() const
{
    return returnType;
}

std::string FuncDecStmt::translateToCpp() const
{
    std::string out = returnType.translateTypeToCpp() + " " + funcName + "(";
    bool first = true;
    for (auto& param : params)
    {
        if (!first)
        {
            out += ", ";
        }
        out += param->getType().translateTypeToCpp(); + " " + Utils::wstrToStr(param->getName());
        first = false;
    }
    out += ");";
    return out;
}
