#include "CircularDependency.h"

CircularDependency::CircularDependency(const Token& token, const std::filesystem::path& path1,
const std::filesystem::path& path2) : Error(token, "Creating circular include: \"" + path1.string() + "\" -> \"" + path2.string() + "\"")
{
}
