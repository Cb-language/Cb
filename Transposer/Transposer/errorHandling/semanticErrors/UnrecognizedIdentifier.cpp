#include "UnrecognizedIdentifier.h"

UnrecognizedIdentifier::UnrecognizedIdentifier(const Token& token) : Error(token, "this identifier is not recognized")
{
}
