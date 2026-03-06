#pragma once
#include "errorHandling/Error.h"

class UnimplementedPureVirtualMethod : public Error
{
public:
    UnimplementedPureVirtualMethod(const Token& token, const std::string& methodName);
};
