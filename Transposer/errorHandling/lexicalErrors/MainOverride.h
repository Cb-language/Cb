#pragma once
#include "errorHandling/Error.h"

class MainOverride : Error
{
public:
    MainOverride(const Token &token);
};
