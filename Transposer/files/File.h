#pragma once
#include <filesystem>

#include "parser/Parser.h"

class File
{
private:
    friend class FileNode;

    const std::filesystem::path inPath;
    const std::filesystem::path outPath;
    Parser parser;

    bool readIncludes = false;
    bool parsed = false;
    bool analyzed = false;

    std::vector<Token> tokenize() const;

    static std::filesystem::path mainPath;

    File(const std::wstring& inPath, const std::wstring& outPath);
    const std::filesystem::path& getInPath() const;
    const std::filesystem::path& getOutPath() const;
    const std::vector<std::pair<std::filesystem::path, Token>>& getIncludes();
    void parse();
    void analyze();
    void write() const;

    static void setMainPath(const std::filesystem::path& mainPath);
};
