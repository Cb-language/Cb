#include "MisplacedKeyword.h"

MisplacedKeyword::MisplacedKeyword(const int line, const int column, const std::string &token) : Error(line, column, "misplaced keyword", token)
{
}