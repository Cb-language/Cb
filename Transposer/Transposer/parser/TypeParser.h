#pragma once
#include "ParserContext.h"

class TypeParser
{
private:
    ParserContext& c;

public:
    explicit TypeParser(ParserContext& c);

    std::unique_ptr<IType> parseIType();
    std::unique_ptr<IType> parseType() const;
    std::unique_ptr<IType> parseArrayType();
    std::unique_ptr<IType> parseClassType() const;

    void parse() const;
};
