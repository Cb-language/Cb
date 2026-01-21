#include "FileGraph.h"

FileGraph& FileGraph::Instance()
{
    static FileGraph instance;
    return instance;
}

FileNode* FileGraph::GetMain() const
{
    return FileGraph::Instance().main;
}

void FileGraph::reset()
{
    if (main != nullptr)
    {
        FileNode::clear(main);
        main = nullptr;
    }
}

void FileGraph::build(const std::filesystem::path& main)
{
    this->main = FileNode::build(main);
}

void FileGraph::start() const
{
    main->start();
}

FileGraph::~FileGraph()
{
    reset();
}
