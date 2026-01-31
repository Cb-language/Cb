#pragma once
#include "errorHandling/Error.h"

class CouldntOpenFileForWriting : public Error
{
public:
    CouldntOpenFileForWriting(const Token& token, const std::filesystem::path& path);
};
