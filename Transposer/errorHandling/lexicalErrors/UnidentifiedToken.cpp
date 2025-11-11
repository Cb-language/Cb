#include "UnidentifiedToken.h"

UnidentifiedToken::UnidentifiedToken(const int line, const int column, const std::string &token) : Error(line, column,"unidentified token", token)
{
}