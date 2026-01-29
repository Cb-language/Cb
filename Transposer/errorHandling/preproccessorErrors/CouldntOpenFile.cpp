#include "CouldntOpenFile.h"

CouldntOpenFile::CouldntOpenFile(const Token& token, const std::filesystem::path& path)
    : Error(token, "Unexpected Error: couldn't open file: \"" + path.string() + "\"")
{
}