#include "CMDFactory.h"

#ifdef _WIN32
#include "WindowsCMD.h"
#else
#include "UnixCMD.h"
#endif

std::unique_ptr<CMD> CMDFactory::createCMD()
{
#ifdef _WIN32
    return std::make_unique<WindowsCMD>();
#else
    return std::make_unique<UnixCMD>();
#endif
}