#include "MissingIdentifier.h"

MissingIdentifier::MissingIdentifier(const int line, const int column, const std::string &token) : Error(line, column, "", token)
{
}