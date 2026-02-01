#pragma once
#include "errorHandling/Error.h"

class StmtInsideSwitchThatIsNotCase : public Error
{
public:
    StmtInsideSwitchThatIsNotCase(const Token& token);
};
