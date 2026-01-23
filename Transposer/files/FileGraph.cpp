#include "FileGraph.h"

#include <ranges>

FileGraph& FileGraph::Instance()
{
    static FileGraph instance;
    return instance;
}

FileNode* FileGraph::GetMain() const
{
    return main;
}

void FileGraph::reset()
{
    if (main != nullptr)
    {
        FileNode::clear(main);
        main = nullptr;
    }
}

void FileGraph::build(const std::filesystem::path& main, const std::filesystem::path& outDir)
{
    File::setMainPath(main);
    File::setOutDir(outDir);
    this->main = FileNode::build(main.filename(), outDir.filename());
}

void FileGraph::start() const
{
    main->start();
}

void FileGraph::write() const
{
    main->write();
}

std::vector<std::filesystem::path> FileGraph::getAllCppPaths()
{
    return FileNode::getAllCppPath();
}

std::filesystem::path FileGraph::getOutPath(const std::filesystem::path& inPath)
{
    auto normalize = [](const std::filesystem::path& p) {
        std::filesystem::path np;
        for (auto& part : p)
            np /= part;
        return np;
    };

    for (const auto& [path, node] : FileNode::nodes)
    {
        if (normalize(path) == normalize(File::getMainPath() / inPath))
        {
            return node->file.outPathH;
        }
    }

    return "";
}

FileGraph::~FileGraph()
{
    reset();
}
