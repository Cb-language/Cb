#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../../../symbols/Type/Type.h"
#include "AST/abstract/Call.h"
#include "symbols/Func.h"

class FuncCallExpr : public Call
{
private:
    const std::string name;
    std::vector<std::unique_ptr<Expr>> args;
    std::unique_ptr<IType> type;
    bool needsSemicolon;

public:
    FuncCallExpr(const Token& token, IFuncDeclStmt* funcDecl,
        const std::string& name, std::vector<std::unique_ptr<Expr>> args, const bool needsSemicolon, ClassDeclStmt* classDecl = nullptr);

    std::unique_ptr<IType> getType() const override;
    void analyze() const override;
    std::string translateToCpp() const override;

    void setType(std::unique_ptr<IType> type);
    bool isLegalCall(const Func& func) const;

    const Token& getToken() const;
    const std::string& getName() const;

    std::string toString() const override;
    void setNeedsSemicolon(const bool needsSemicolon) override;
};
