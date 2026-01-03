#pragma once
#include "errorHandling/Error.h"

class IllegalOpOnType : public Error
{
public:
    IllegalOpOnType(const Token& token, const std::string& type);
    IllegalOpOnType(const Token& token, const std::string& type1, const std::string& type2, const std::string& op);
};
