#pragma once
#include "ParserContext.h"

class TypeParser
{
private:
    ParserContext& c;

    // std::unique_ptr<IType> parseIType();
    // std::unique_ptr<Type> parseType();
    // std::unique_ptr<ArrayType> parseArrayType();
    // std::unique_ptr<ClassType> parseClassType();

public:
    explicit TypeParser(ParserContext& c);

    void parse() const;
};
