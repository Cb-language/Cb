#include "FileGraph.h"

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
    this->main = FileNode::build(main.filename());
}

void FileGraph::start() const
{
    main->start();
}

FileGraph::~FileGraph()
{
    reset();
}
