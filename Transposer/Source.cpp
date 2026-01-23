#include <iostream>
#include <sstream>
#include <windows.h>
#include "token/Tokenizer.h"
#include "errorHandling/Error.h"
#include "files/ArrayHelper.h"
#include "files/FileGraph.h"
#include "parser/Parser.h"

#ifdef _WIN32
#include <windows.h>
void EnableANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, mode);
}
#endif

enum Mode
{
    TRANSLATE,
    COMPILE,
    RUN
};

int main(int argc, char* argv[])
{
#ifdef _WIN32
    EnableANSI();
#endif
    // Enable UTF-8 output (mostly for other parts that may use cout)
    SetConsoleOutputCP(CP_UTF8);

    // Initialize tokenizer
    Tokenizer::init();

    // Check for correct number of arguments
    if (argc < 3 || argc > 4)
    {
        std::cout << "Usage: <main_path> <mode> [save_path]" << std::endl;
        return 1;
    }

    // Create FileManager instance and perform file operations
    std::string inPath = std::string(argv[1]);
    std::string outPath;
    if (argc == 4)
    {
        outPath = std::string(argv[3]);
    }

    // Determine mode of operation
    std::string arg = argv[2];
    int mode = arg == "T" ? TRANSLATE :
               arg == "R" ? RUN :
               arg == "C" ? COMPILE : -1;

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
    default:
        std::cout << "Invalid mode. Use T for Translate, C for Compile, R for Run." << std::endl;
    }

    if (mode != TRANSLATE && std::system("g++ --version >nul 2>&1") != 0)
    {
        std::cerr << "g++ not installed" << std::endl;
    }

    FileGraph& graph = FileGraph::Instance();
    graph.reset();
    try
    {
        Utils::logMsg("Translating...");
        graph.build(inPath, outPath);
        graph.start();
        graph.write();
        ArrayHelper::write(File::getOutDir());
    }
    catch (const Error& err)
    {
        std::cerr << err.what() << std::endl;
        graph.reset();
        return -1;
    }

    std::filesystem::path exePath = outPath;
    exePath = exePath.replace_extension(".exe");

    if (mode == COMPILE || mode == RUN)
    {
        std::ostringstream cmd;

        std::string filesStr = "";
        const auto paths = FileGraph::getAllCppPaths();

        for (const auto& path : paths)
        {
            filesStr += " \"" + path.string() + "\" ";
        }

        cmd << "g++ -static -static-libgcc -static-libstdc++ -pthread"
        << filesStr << "-Iincludes -o " << exePath << std::endl;

        Utils::logMsg("Compiling...");

        if (std::system(cmd.str().c_str()) != 0)
        {
            std::cerr << "Error with g++ : command: " << cmd.str() << std::endl;
            graph.reset();
            return -4;
        }
    }

    graph.reset();

    if (mode == RUN)
    {
        std::ostringstream cmd;
        Utils::logMsg("Running...");

        cmd << "start \"\" cmd /c \"" << exePath << " & pause\"";

        std::system(cmd.str().c_str());
    }

    return 0;
}
