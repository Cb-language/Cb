#include "FuncDeclStmt.h"

FuncDeclStmt::FuncDeclStmt(Scope *scope, const std::string &funcName, const Type &returnType, std::vector<std::unique_ptr<Var>> &&params) : Stmt(scope, nullptr),
    funcName(funcName), returnType(returnType)
{
    for (auto &param: params)
    {
        this->params.emplace_back(std::move(param));
    }
}

const std::vector<std::unique_ptr<Var>>& FuncDeclStmt::getParams() const
{
    return params;
}

std::string FuncDeclStmt::getName() const
{
    return funcName;
}

Type FuncDeclStmt::getReturnType() const
{
    return returnType;
}

bool FuncDeclStmt::isLegal() const
{
    
    return true;
}


std::string FuncDeclStmt::translateToCpp() const
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
