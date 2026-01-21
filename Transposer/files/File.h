#pragma once
#include <filesystem>

#include "parser/Parser.h"

class File
{
private:
    const std::filesystem::path inPath;
    const std::filesystem::path outPath;
    Parser parser;

    std::vector<Token> tokenize() const;

    static std::filesystem::path mainPath;
public:
    File(const std::wstring& inPath, const std::wstring& outPath);
    const std::filesystem::path& getInPath() const;
    const std::filesystem::path& getOutPath() const;

    static void setMainPath(const std::filesystem::path& mainPath);
};
