#pragma once
#include "errorHandling/Error.h"

class CircularDependency : public Error
{
public:
    CircularDependency(const Token& token,  const std::filesystem::path& path1,  const std::filesystem::path& path2);
};
