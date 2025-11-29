#include "InvalidStatement.h"

InvalidStatement::InvalidStatement(const Token &token) : Error(token, "invalid statement")
{
}