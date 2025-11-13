#include "UnidentifiedToken.h"

UnidentifiedToken::UnidentifiedToken(const Token &token) : Error(token, "unidentified token")
{
}