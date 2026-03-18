#include "CaseNotInsideIf.h"

CaseNotInsideIf::CaseNotInsideIf(const Token& token) : Error(token, "an else statment must come after an if statement")
{
}
