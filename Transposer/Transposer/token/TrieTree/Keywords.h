#pragma once
#include <vector>

#include "KeywordInfo.h"

extern const std::vector<KeywordInfo> KEYWORDS;

inline constexpr std::string COMMENT_MULTY_END = "*?";
inline constexpr std::string CONST_STR_CLOSE = "\"";
inline constexpr std::string CONST_CHAR_CLOSE = "\'";