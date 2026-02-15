#include <iostream>
#include <sstream>
#include <filesystem>
#include "token/Tokenizer.h"
#include "errorHandling/Error.h"
#include "files/ArrayHelper.h"
#include "files/FileGraph.h"
#include "parser/Parser.h"
#include "multyOSSupport/CMDFactory.h"

enum Mode
{
    TRANSLATE, COMPILE, RUN, LSP
};

int main(int argc, char* argv[])
{
    std::unique_ptr<CMD> cmd = CMDFactory::createCMD();

    cmd->setupConsole();

    Tokenizer::init();

    if (argc != 4)
    {
        std::cout << "Usage: <main_path> <mode> [save_path]" << std::endl;
        return 1;
    }

    auto inPath = std::string(argv[1]);
    std::string outPath;
    if (argc == 4) outPath = std::string(argv[3]);

    std::string arg = argv[2];
    int mode = arg == "T" ? TRANSLATE :
               arg == "R" ? RUN :
               arg == "C" ? COMPILE :
               arg == "LSP" ? LSP : -1;

    switch (mode)
    {
        case TRANSLATE:
            Utils::logMsg("Translating Mode");
            break;

        case RUN:
            Utils::logMsg("Running Mode");
            break;

        case COMPILE:
            Utils::logMsg("Compiling Mode");
            break;

        case LSP:
            Utils::logMsg("LSP Mode");

        default:
            std::cout << "Invalid mode. Use T, C, or R." << std::endl;
            return 1;
    }

    if (mode != TRANSLATE && std::system(("g++ --version " + cmd->getNullDevice()).c_str()) != 0)
    {
        std::cerr << "g++ not installed" << std::endl;
        return -1;
    }

    FileGraph& graph = FileGraph::Instance();
    graph.reset();
    Utils::logMsg("Translating...");
    graph.build(inPath, outPath);
    graph.start();
    
    std::vector<Error*> errors = FileGraph::getAllErrors();

    if (!errors.empty())
    {
        for (const auto& err : errors)
        {
            std::cerr << err->what() << std::endl;
        }
        graph.reset();
        SymbolTable::clearClasses();
        return -1;
    }

    if (mode != LSP) graph.write();
    ArrayHelper::write(File::getOutDir());

    std::filesystem::path exePath = outPath;
    std::string ext = cmd->getExeExtension();
    if (!ext.empty())
    {
        exePath.replace_extension(ext);
    }
    else
    {
        exePath.replace_extension("");
    }

    if (mode == COMPILE || mode == RUN)
    {
        std::ostringstream cmdBuild;
        std::string filesStr;
        const auto paths = FileGraph::getAllCppPaths();

        for (const auto& path : paths)
        {
            filesStr += " \"" + path.string() + "\" ";
        }

        cmdBuild << "g++ -pthread " << filesStr
                 << "-Iincludes -o \"" << exePath.string() << "\""
                 << cmd->getCompileFlags();

        Utils::logMsg("Compiling...");

        if (std::system(cmdBuild.str().c_str()) != 0)
        {
            std::cerr << "Error with g++ : command: " << cmdBuild.str() << std::endl;
            graph.reset();
        SymbolTable::clearClasses();
            return -4;
        }
    }

    graph.reset();
    SymbolTable::clearClasses();

    if (mode == RUN)
    {
        Utils::logMsg("Running...");
        cmd->runExecutable(exePath.string());
    }

    return 0;
}