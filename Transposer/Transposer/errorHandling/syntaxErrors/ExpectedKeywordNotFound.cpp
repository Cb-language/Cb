#include "ExpectedKeywordNotFound.h"

ExpectedKeywordNotFound::ExpectedKeywordNotFound(const Token& token, const std::string& expectedKeyword) : Error(token, "The expected keyword: " + expectedKeyword + "was not found")
{
}
