#include "UnexpectedEOF.h"

UnexpectedEOF::UnexpectedEOF(const Token &token) : Error(token, "Unexpected end of file")
{
}
