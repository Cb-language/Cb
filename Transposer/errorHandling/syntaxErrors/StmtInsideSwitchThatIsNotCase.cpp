#include "StmtInsideSwitchThatIsNotCase.h"

StmtInsideSwitchThatIsNotCase::StmtInsideSwitchThatIsNotCase(const Token& token) : Error(token, "Can't use any statement beside case (C) inside switch")
{
}
