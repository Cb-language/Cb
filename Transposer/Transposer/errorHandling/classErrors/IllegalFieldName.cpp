#include "IllegalFieldName.h"

IllegalFieldName::IllegalFieldName(const Token& token) : Error(token, "field's name must start with a note")
{
}