#include "FileNode.h"

#include <ranges>

#include "errorHandling/preproccessorErrors/CircularDependency.h"
#include "errorHandling/preproccessorErrors/IncludePathNotFound.h"

std::unordered_map<
        std::filesystem::path,
        std::unique_ptr<FileNode>
    > FileNode::nodes;

FileNode::FileNode(const std::filesystem::path& inFilename, const std::filesystem::path& outFilename)
    : file(File(inFilename.c_str(),outFilename))
{
}

FileNode::FileNode(const std::filesystem::path& filename)
    : file(File(filename))
{
}

void FileNode::readAndAddChildren()
{
    for (const auto res = file.getIncludes(); const auto& [first, second] : res)
    {
        const std::filesystem::path fullPath =
            std::filesystem::weakly_canonical(
                File::getMainPath() / first
            );

        const Token& t = second;

        if (!std::filesystem::exists(fullPath))
        {
            file.parser.addError(std::make_unique<IncludePathNotFound>(t, fullPath));
            continue;
        }

        if (!canAdd(fullPath))
        {
            file.parser.addError(std::make_unique<CircularDependency>(t, file.getInPath(), fullPath));
            continue;
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

    if (const auto it = nodes.find(canonical); it != nodes.end())
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

    if (const auto it = nodes.find(canonicalIn); it != nodes.end())
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

void FileNode::analyzePass1(SymbolTable& symTable)
{
    file.analyzePass1(symTable);

    for (auto& child : children)
    {
        child->analyzePass1(symTable);
    }
}

void FileNode::analyzePass2(SymbolTable& symTable)
{
    file.analyzePass2(symTable);

    for (auto& child : children)
    {
        child->analyzePass2(symTable);
    }
}

void FileNode::analyzePass3(SymbolTable& symTable)
{
    file.analyzePass3(symTable);

    for (auto& child : children)
    {
        child->analyzePass3(symTable);
    }
}

void FileNode::write(const bool isMain)
{
    for (auto& child : children) child->write();
    file.write(isMain);
}

void FileNode::clear(const FileNode* main)
{
    if (main == nullptr) return;

    // clear all internal raw pointers
    for (const auto& node : nodes | std::views::values)
    {
        node->children.clear();
        node->parents.clear();
    }

    nodes.clear(); // deletes all FileNode objects safely
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

const std::vector<std::unique_ptr<Error>>& FileNode::getErrors() const
{
    return file.getErrors();
}

std::vector<Error*> FileNode::getAllErrors()
{
    std::vector<Error*> errors;
    for (const auto& node : nodes | std::views::values)
    {
        for (const auto& err : node->getErrors())
        {
            errors.push_back(err.get());
        }
    }
    return errors;
}
