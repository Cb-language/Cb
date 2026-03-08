#pragma once
#include "WordBoundary.h"
#include "token/Token.h"

struct KeywordInfo
{
    TokenType type;
    std::string keyword;
    WordBoundary boundary;
};
