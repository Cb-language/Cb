#include "FileGraph.h"

#include <ranges>

#include "errorHandling/entryPointErrors/NoMain.h"

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

    if (!main->hasMain())
    {
        throw NoMain(main->file.parser.getLast());
    }
}

void FileGraph::write() const
{
    main->write(true);
}

std::vector<std::filesystem::path> FileGraph::getAllCppPaths()
{
    return FileNode::getAllCppPath();
}

std::vector<Error*> FileGraph::getAllErrors()
{
    return FileNode::getAllErrors();
}

FileGraph::~FileGraph()
{
    reset();
}
