#include "InvalidOverrideSignature.h"

InvalidOverrideSignature::InvalidOverrideSignature(const Token& token) : Error(token, "override Signature does not signature of the base method")
{
}