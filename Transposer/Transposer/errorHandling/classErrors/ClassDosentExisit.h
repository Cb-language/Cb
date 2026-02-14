#pragma once
#include "errorHandling/Error.h"

class ClassDosentExisit : public Error
{
public:
    ClassDosentExisit(const Token& token, const std::string& className);
};
