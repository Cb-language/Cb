#include "NoCtor.h"

NoCtor::NoCtor(const Token& token) : Error(token, "No Constructor Available for this class")
{
}
