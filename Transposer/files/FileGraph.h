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
    void build(const std::filesystem::path& main);
    void start() const;

private:
    ~FileGraph();
    FileNode* main = nullptr;
};
