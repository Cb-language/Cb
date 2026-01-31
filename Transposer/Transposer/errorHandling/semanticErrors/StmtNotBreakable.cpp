#include "StmtNotBreakable.h"

StmtNotBreakable::StmtNotBreakable(const Token& token) : Error(token, "pause inside a non pause-able statement")
{
}