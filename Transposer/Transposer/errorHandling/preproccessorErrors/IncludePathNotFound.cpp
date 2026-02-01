#include "IncludePathNotFound.h"

IncludePathNotFound::IncludePathNotFound(const Token& token,  const std::filesystem::path& Path) : Error(token, "Include file not found: \"" + Path.string() + "\"")
{
}
