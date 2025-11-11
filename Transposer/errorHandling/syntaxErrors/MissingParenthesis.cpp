#include "MissingParenthesis.h"

MissingParenthesis::MissingParenthesis(const int line, const int column, const std::string &token) : Error(line, column, "missing parenthesis", token)
{
}