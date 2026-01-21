#pragma once
#include <set>
#include <unordered_map>

#include "File.h"

class FileNode
{
private:
    File file;

    std::set<FileNode*> children;
    std::set<FileNode*> parents;

    static std::unordered_map<
        std::filesystem::path,
        std::unique_ptr<FileNode>
    > nodes;

    static bool first;

    explicit FileNode(const std::filesystem::path& path);

    void readAndAddChildren();

    bool canAdd(const std::filesystem::path& path) const;

    static FileNode* getNode(const std::filesystem::path& path);

    static void deleteNode(FileNode* node);

public:
    static FileNode* build(const std::filesystem::path& path);

    static void clear(FileNode* main);
};
