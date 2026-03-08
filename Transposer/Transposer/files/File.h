#pragma once
#include <filesystem>

#include "parser/Parser.h"
#include "token/Tokenizer.h"

class File
{
private:
    friend class FileNode;
    friend class FileGraph;
    Tokenizer tokenizer;

    const std::filesystem::path inPath;
    const std::filesystem::path outPathH;
    const std::filesystem::path outPathCpp;
    Parser parser;

    std::vector<std::pair<std::filesystem::path, Token>> includes;
    bool readIncludes = false;
    bool parsed = false;
    bool analyzed = false;
    bool writen = false;

    std::vector<Token> tokenize() const;

    static std::filesystem::path mainPath;
    static std::filesystem::path outDir;

    File(const std::string& inFilename, const std::string& outFilename);
    File(const std::filesystem::path& inPath, const std::filesystem::path& outPath);
    explicit File(const std::filesystem::path& path);
    const std::filesystem::path& getInPath() const;
    const std::filesystem::path& getOutHPath() const;
    const std::filesystem::path& getOutCppPath() const;
    const std::vector<std::pair<std::filesystem::path, Token>>& getIncludes();
    void parse();
    void analyze();
    void write(const bool isMain = false);
    const std::vector<std::unique_ptr<Error>>& getErrors() const;

public:
    static void setMainPath(const std::filesystem::path& mainPath);
    static const std::filesystem::path& getMainPath();

    static void setOutDir(const std::filesystem::path& outDir);
    static const std::filesystem::path& getOutDir();

    Parser& getParser();
};
