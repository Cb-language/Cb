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

void FileGraph::start()
{
    main->analyzePass1(symTable);
    main->analyzePass2(symTable);
    main->analyzePass3(symTable);

    if (const auto path = symTable.getMainPath(); !path.has_value() || path != main->file.getInPath())
    {
        symTable.addError(std::make_unique<NoMain>(main->file.parser.getContext().getFirstToken()));
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

std::vector<Error*> FileGraph::getAllErrors() const
{
    auto err = FileNode::getAllErrors();

    for (auto& e : symTable.getErrors())
    {
        err.emplace_back(e);
    }

    return err;
}

FileGraph::~FileGraph()
{
    reset();
}
