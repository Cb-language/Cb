#pragma once
#include "WordBoundary.h"
#include "token/Token.h"

struct KeywordInfo
{
    CbTokenType type;
    std::string keyword;
    WordBoundary boundary;
};
