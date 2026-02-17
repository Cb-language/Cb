#include "CallWithoutCopyright.h"

CallWithoutCopyright::CallWithoutCopyright(const Token& token, const std::string& funcName) : Error(token, "Called a song (" + funcName + ") without copyrighting it")
{
}
