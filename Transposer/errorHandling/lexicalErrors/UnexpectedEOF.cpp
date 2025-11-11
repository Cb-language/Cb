#include "UnexpectedEOF.h"

UnexpectedEOF::UnexpectedEOF(const int line, const int column, const std::string &token) : Error(line, column, "unexpected end of file", token)
{
}
