#include "InvalidExpression.h"

InvalidExpression::InvalidExpression(const Token &token) : Error(token, "invalid expression")
{
}