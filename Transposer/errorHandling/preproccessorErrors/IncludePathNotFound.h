#pragma once
#include "errorHandling/Error.h"

class IncludePathNotFound : public Error
{
public:
    IncludePathNotFound(const Token& token,  const std::filesystem::path& Path);
};
