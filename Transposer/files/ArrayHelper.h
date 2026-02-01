#pragma once
#include <filesystem>
#include <string>

class ArrayHelper
{
private:
    const static std::string hStr;
    const static std::string tppStr;
    static bool wrote;

public:
    static void write(const std::filesystem::path& dir);
};
