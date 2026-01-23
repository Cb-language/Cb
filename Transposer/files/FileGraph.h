#pragma once
#include "FileNode.h"

class FileGraph
{
public:
    static FileGraph& Instance();

    FileGraph() = default;
    FileGraph(const FileGraph&) = delete;
    FileGraph& operator=(const FileGraph&) = delete;
    FileGraph(FileGraph&&) = delete;
    FileGraph& operator=(FileGraph&&) = delete;

    FileNode* GetMain() const;

    void reset();
    void build(const std::filesystem::path& main, const std::filesystem::path& outDir);
    void start() const;
    void write() const;
    static std::vector<std::filesystem::path> getAllCppPaths();

    static std::filesystem::path getOutPath(const std::filesystem::path& inPath);

private:
    ~FileGraph();
    FileNode* main = nullptr;
};
