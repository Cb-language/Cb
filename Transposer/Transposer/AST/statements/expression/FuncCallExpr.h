#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../../../symbols/Type/PrimitiveType.h"
#include "AST/abstract/Call.h"
#include "symbols/Func.h"

class FuncCallExpr : public Call
{
private:
    const FQN name;
    std::vector<std::unique_ptr<Expr>> args;
    std::unique_ptr<IType> type;
    bool needsSemicolon;
    IFuncDeclStmt* funcDecl;
    const ClassNode* targetClass = nullptr;

public:
    FuncCallExpr(const Token& token,
        const FQN& name, std::vector<std::unique_ptr<Expr>> args, const bool needsSemicolon);

    std::unique_ptr<IType> getType() const override;
    void analyze() const override;
    std::string translateToCpp() const override;

    void setType(std::unique_ptr<IType> type);
    void setClassDecl(IFuncDeclStmt& decl);
    void setTargetClass(const ClassNode* targetClass);
    bool isLegalCall(const Func& func) const;
    bool argsMatch(const Func& func) const;

    const Token& getToken() const override;
    const FQN& getName() const;

    std::string toString() const override;
    void setNeedsSemicolon(const bool needsSemicolon) override;
};
