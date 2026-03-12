#pragma once
#include "errorHandling/Error.h"

class StmtInsideSwitchThatIsNotCase : public Error
{
public:
    explicit StmtInsideSwitchThatIsNotCase(const Token& token);
};
