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

    std::vector<std::pair<std::filesystem::path, Token>> includes;
    bool readIncludes = false;
    bool parsed = false;
    bool analyzed = false;

    std::vector<Token> tokenize() const;

    static std::filesystem::path mainPath;
    static std::filesystem::path outDir;

    File(const std::wstring& inFilename, const std::wstring& outFilename);
    explicit File(const std::filesystem::path& path);
    const std::filesystem::path& getInPath() const;
    const std::filesystem::path& getOutPath() const;
    const std::vector<std::pair<std::filesystem::path, Token>>& getIncludes();
    void parse();
    void analyze();
    void write() const;

public:
    static void setMainPath(const std::filesystem::path& mainPath);
    static const std::filesystem::path& getMainPath();

    static void setOutDir(const std::filesystem::path& outDir);
    static const std::filesystem::path& getOutDir();
};
