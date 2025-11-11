#include "InvalidStatement.h"

InvalidStatement::InvalidStatement(const int line, const int column, const std::string &token) : Error(line, column, "invalid statement", token)
{
}