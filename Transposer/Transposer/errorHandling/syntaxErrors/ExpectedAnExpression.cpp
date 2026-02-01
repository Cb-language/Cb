#include "ExpectedAnExpression.h"

ExpectedAnExpression::ExpectedAnExpression(const Token& token) : Error(token, "Expected an Expression")
{
}
