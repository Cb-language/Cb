#pragma once
#include "errorHandling/Error.h"

class CouldntOpenFile : public Error
{
public:
    CouldntOpenFile(const Token& token, const std::filesystem::path& path);
};