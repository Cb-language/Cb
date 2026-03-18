#pragma once
#include "errorHandling/Error.h"

class IllegalDotOpError : public Error
{
public:
    IllegalDotOpError(const Token& token, const std::string& left, const std::string& right);
    IllegalDotOpError(const Token& token, const std::string& left, const std::string& right, const std::string& leftType);
};
