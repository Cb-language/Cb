#include "StmtNotContinueAble.h"

StmtNotContinueAble::StmtNotContinueAble(const Token& token) : Error(token, "resume inside a non resume-able statement")
{
}