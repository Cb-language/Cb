#pragma once
#include <string>
#include <vector>
#include "errorHandling/Error.h"

class LSPPacker
{
public:
    static std::string pack(const std::vector<Error*>& errors);
    static void deleteErrors(const std::vector<Error*>& errors);
};
