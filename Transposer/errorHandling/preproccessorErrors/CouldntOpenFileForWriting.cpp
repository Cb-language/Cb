#include "CouldntOpenFileForWriting.h"

CouldntOpenFileForWriting::CouldntOpenFileForWriting(const Token& token, const std::filesystem::path& path)
    : Error(token, "Could not open file for writing:\"" + path.string() + "\"")
{
}
