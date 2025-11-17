#pragma once
#include "AST/abstract/Statement.h"

class Constant_Value_Stmt : public Stmt {
public:
    Constant_Value_Stmt(const std::wstring& value);
    bool isLegal() const override;
    std::string translateToCpp() const override;
    std::wstring getValue() const;
private:
    std::wstring value;

};