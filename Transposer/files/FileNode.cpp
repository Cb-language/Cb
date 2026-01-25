#include "FileNode.h"

#include <ranges>

#include "errorHandling/Error.h"

std::unordered_map<
        std::filesystem::path,
        std::unique_ptr<FileNode>
    > FileNode::nodes;

FileNode::FileNode(const std::filesystem::path& inPath, const std::filesystem::path& outFilename)
    : file(File(inPath.c_str(),outFilename))
{
}

FileNode::FileNode(const std::filesystem::path& filename)
    : file(File(filename))
{
}

void FileNode::readAndAddChildren()
{
    const auto res = file.getIncludes();

    for (const auto& p : res)
    {
        const std::filesystem::path fullPath =
            std::filesystem::weakly_canonical(
                File::getMainPath() / p.first
            );

        const Token& t = p.second;

        if (!std::filesystem::exists(fullPath))
        {
            throw Error(
                t,
                "Include file not found: \"" + fullPath.string() + "\""
            );
        }

        if (!canAdd(fullPath))
        {
            throw Error(
                t,
                "Creating circular include: \"" +
                file.getInPath().string() +
                "\" -> \"" +
                fullPath.string() +
                "\""
            );
        }

        FileNode* f = getNode(fullPath);

        children.emplace(f);
        f->parents.emplace(this);
    }
}


bool FileNode::canAdd(const std::filesystem::path& path) const
{
    if (file.getInPath() == path) return false;

    for (const FileNode* p : parents)
    {
        if (!p->canAdd(path))
        {
            return false;
        }
    }

    return true;
}


FileNode* FileNode::getNode(const std::filesystem::path& path)
{
    const std::filesystem::path canonical =
        std::filesystem::weakly_canonical(path);

    auto it = nodes.find(canonical);
    if (it != nodes.end())
    {
        return it->second.get();
    }

    auto node = std::unique_ptr<FileNode>(new FileNode(canonical));
    FileNode* raw = node.get();

    nodes.emplace(canonical, std::move(node));
    return raw;
}

FileNode* FileNode::getNode(const std::filesystem::path& inPath, const std::filesystem::path& outPath)
{
    const std::filesystem::path canonicalIn =
        std::filesystem::weakly_canonical(inPath);
    const std::filesystem::path canonicalOut =
        std::filesystem::weakly_canonical(outPath);

    auto it = nodes.find(canonicalIn);
    if (it != nodes.end())
    {
        return it->second.get();
    }

    auto node = std::unique_ptr<FileNode>(new FileNode(canonicalIn, canonicalOut));
    FileNode* raw = node.get();

    nodes.emplace(canonicalIn, std::move(node));
    return raw;
}

FileNode* FileNode::build(const std::filesystem::path& path)
{
    // Get or create the node for this path
    FileNode* node = getNode(path);

    // If the node already has children, we have already built its subtree
    // This prevents infinite recursion on circular includes (though canAdd already prevents them)
    if (!node->children.empty())
    {
        return node;
    }

    // Read the file and recursively add children
    node->readAndAddChildren();

    // Recursively build the include graph for each child
    for (const FileNode* child : node->children)
    {
        build(child->file.getInPath());
    }

    return node;
}

FileNode* FileNode::build(const std::filesystem::path& inPath, const std::filesystem::path& outPath)
{
    // Get or create the node for this path
    FileNode* node = getNode(inPath, outPath);

    // If the node already has children, we have already built its subtree
    // This prevents infinite recursion on circular includes (though canAdd already prevents them)
    if (!node->children.empty())
    {
        return node;
    }

    // Read the file and recursively add children
    node->readAndAddChildren();

    // Recursively build the include graph for each child
    for (const FileNode* child : node->children)
    {
        build(child->file.getInPath());
    }

    return node;
}

void FileNode::start()
{
    for (auto& child : children) child->start();

    file.parse();
    file.analyze();
}

void FileNode::write(const bool isMain)
{
    for (auto& child : children) child->write();
    file.write(isMain);
}

void FileNode::clear(FileNode* main)
{
    if (main == nullptr) return;
    deleteNode(main);

    nodes.clear();
}

std::vector<std::filesystem::path> FileNode::getAllCppPath()
{
    std::vector<std::filesystem::path> v;
    for (const auto& node : nodes | std::views::values)
    {
        v.emplace_back(node->file.outPathCpp);
    }

    return v;
}

void FileNode::deleteNode(FileNode* node)
{
    for (auto& child : node->children)
    {
        deleteNode(child);
    }

    node->children.clear();
    node->parents.clear();
}
