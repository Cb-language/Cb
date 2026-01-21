#pragma once
#include <set>
#include <unordered_map>

#include "File.h"

class FileNode
{
private:
    friend class FileGraph;
    File file;

    std::set<FileNode*> children;
    std::set<FileNode*> parents;

    static std::unordered_map<
        std::filesystem::path,
        std::unique_ptr<FileNode>
    > nodes;

    static bool first;

    FileNode(const std::filesystem::path& inPath, const std::filesystem::path& outPath);
    explicit FileNode(const std::filesystem::path& path);

    void readAndAddChildren();

    bool canAdd(const std::filesystem::path& path) const;

    static FileNode* getNode(const std::filesystem::path& path);

    static void deleteNode(FileNode* node);

    static FileNode* build(const std::filesystem::path& path);

    void start();

    static void clear(FileNode* main);
};
