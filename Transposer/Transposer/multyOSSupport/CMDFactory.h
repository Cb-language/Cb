#pragma once
#include <memory>
#include "CMD.h"

class CMDFactory
{
public:
    static std::unique_ptr<CMD> createCMD();
};

