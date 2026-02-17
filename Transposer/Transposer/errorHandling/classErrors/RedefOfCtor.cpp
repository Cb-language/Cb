#include "RedefOfCtor.h"

RedefOfCtor::RedefOfCtor(const Token& token) : Error(token, "Redefinition of a constractor")
{
}
