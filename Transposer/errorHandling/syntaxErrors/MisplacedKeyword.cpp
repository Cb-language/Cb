#include "MisplacedKeyword.h"

MisplacedKeyword::MisplacedKeyword(const Token &token) : Error(token, "misplaced keyword")
{
}