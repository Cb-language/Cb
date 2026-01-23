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

    FileNode(const std::filesystem::path& inFilename, const std::filesystem::path& outFilename);
    explicit FileNode(const std::filesystem::path& filename);

    void readAndAddChildren();

    bool canAdd(const std::filesystem::path& path) const;

    static FileNode* getNode(const std::filesystem::path& path);
    static FileNode* getNode(const std::filesystem::path& inPath, const std::filesystem::path& outPath);

    static void deleteNode(FileNode* node);

    static FileNode* build(const std::filesystem::path& path);
    static FileNode* build(const std::filesystem::path& inPath, const std::filesystem::path& outPath);

    void start();

    void write(const bool isMain = false);

    static void clear(FileNode* main);

    static std::vector<std::filesystem::path> getAllCppPath();
};
