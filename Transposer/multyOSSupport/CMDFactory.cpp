#include "CMDFactory.h"

#include "WindowsCMD.h"
#include "UnixCMD.h"

std::unique_ptr<CMD> CMDFactory::createCMD()
{
#ifdef _WIN32
    return std::make_unique<WindowsCMD>();
#else
    return std::make_unique<UnixCMD>();
#endif
}