#pragma once
#include "../Error.h"
#include "../../token/Token.h"

class CallWithoutCopyright : public Error
{
public:
    CallWithoutCopyright(const Token& token, const std::string& funcName);
};
