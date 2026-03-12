#pragma once
#include "errorHandling/Error.h"

class MainOverride : public Error
{
public:
    explicit MainOverride(const Token &token);
};
