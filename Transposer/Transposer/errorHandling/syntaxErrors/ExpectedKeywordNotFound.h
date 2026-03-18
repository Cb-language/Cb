#pragma once
#include "errorHandling/Error.h"

class ExpectedKeywordNotFound : public Error
{
public:
    ExpectedKeywordNotFound(const Token& token, const std::string& expectedKeyword);
};
