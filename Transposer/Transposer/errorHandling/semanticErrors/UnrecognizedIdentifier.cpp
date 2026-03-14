#include "UnrecognizedIdentifier.h"

UnrecognizedIdentifier::UnrecognizedIdentifier(const Token& token, const std::string& identifier) : Error(token, "the identifier: " + identifier + " is not recognized")
{
}
