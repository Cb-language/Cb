#pragma once
#include "errorHandling/Error.h"

class MainOverride : public Error
{
public:
    MainOverride(const Token &token);
};
