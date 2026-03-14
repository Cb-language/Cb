#include "IllegalLengthCall.h"

IllegalLengthCall::IllegalLengthCall(const Token& token, const std::string& type) : Error(token, "Can't do duration/sub duration on a non riff type: " + type)
{
}
