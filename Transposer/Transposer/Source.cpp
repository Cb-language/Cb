// ReSharper disable CppDFAUnreachableCode
// ReSharper disable CppDFAConstantConditions
#include <iostream>
#include <sstream>
#include <filesystem>
#include <vector>

#include "token/Tokenizer.h"
#include "errorHandling/Error.h"
#include "files/LibHelper.h"
#include "files/FileGraph.h"
#include "parser/Parser.h"
#include "multyOSSupport/CMDFactory.h"
#include "other/Utils.h"

#include "LSPPacking/LSPPacker.h"

enum Mode
{
    TRANSLATE, COMPILE, RUN, LSP
};

int main(int argc, char* argv[])
{
    std::unique_ptr<CMD> cmd = CMDFactory::createCMD();

    cmd->setupConsole();

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
            break;

        default:
            std::cout << "Invalid mode. Use T, C, or R." << std::endl;
            return 1;
    }

    if (mode != TRANSLATE && std::system(("g++ --version " + cmd->getNullDevice()).c_str()) != 0)
    {
        std::cerr << "g++ not installed" << std::endl;
        return -1;
    }

    ClassTree::init();
    FileGraph& graph = FileGraph::Instance();
    graph.reset();
    if (mode != LSP) Utils::logMsg("Translating...");

    try
    {
        graph.build(inPath, outPath);
        graph.start();
    }
    catch (Error& e) // Catch known semantic/syntax errors (like NoReturn)
    {
        std::vector<Error*> errors = graph.getAllErrors();
        errors.push_back(&e); // Add the fatal error to the list

        if (mode == LSP)
        {
            // Output JSON even if we crashed
            std::cout << LSPPacker::pack(errors) << std::endl;
        }
        else
        {
            for (const auto& err : errors)
            {
                std::cerr << err->what() << std::endl;
            }
        }

        graph.reset();
        SymbolTable::clearClasses();
        return -1;
    }
    catch (const std::exception& e) // Catch unexpected C++ errors
    {
        std::cerr << "Internal Transpiler Error: " << e.what() << std::endl;
        graph.reset();
        SymbolTable::clearClasses();
        return -1;
    }

    if (std::vector<Error*> errors = graph.getAllErrors(); !errors.empty())
    {
        if (mode == LSP)
        {
            std::cout << LSPPacker::pack(errors) << std::endl;
        }
        else
        {
            for (const auto& err : errors)
            {
                std::cerr << err->what() << std::endl;
            }
        }
        graph.reset();
        SymbolTable::clearClasses();
        return -1;
    }

    if (mode != LSP)
    {
        graph.write();
        LibHelper::write(File::getOutDir());
    }

    std::filesystem::path exePath = outPath;
    if (std::string ext = cmd->getExeExtension(); !ext.empty())
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

        for (const auto paths = FileGraph::getAllCppPaths(); const auto& path : paths)
        {
            filesStr += " \"" + path.string() + "\" ";
        }

        for (const auto& path : Utils::getAllObjCppPaths())
        {
            filesStr += " \"" + path.string() + "\" ";
        }

        cmdBuild << "g++ -pthread -std=c++20" << filesStr
                 << "-I" << File::getOutDir() <<" -o \"" << exePath.string() << "\""
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