#include <iostream>
#include <sstream>
#include <filesystem>
#include "token/Tokenizer.h"
#include "errorHandling/Error.h"
#include "files/ArrayHelper.h"
#include "files/FileGraph.h"
#include "parser/Parser.h"

// --- OS SPECIFIC DEFINITIONS ---
#ifdef _WIN32
    #include <windows.h>
    const std::string NULL_DEVICE = ">nul 2>&1";
    const std::string EXE_EXT = ".exe";

    void EnableANSI()
    {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        GetConsoleMode(hOut, &mode);
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, mode);
    }

    void SetupConsole()
    {
        EnableANSI();
        SetConsoleOutputCP(CP_UTF8);
    }
#else
    // Linux implementations
    #include <cstdlib>
    const std::string NULL_DEVICE = ">/dev/null 2>&1";
    const std::string EXE_EXT; // No extension needed on Linux

    void SetupConsole()
    {
        // No action needed here
    }
#endif
// -------------------------------

enum Mode
{
    TRANSLATE,
    COMPILE,
    RUN
};

int main(int argc, char* argv[])
{
    SetupConsole();

    Tokenizer::init();

    if (argc < 3 || argc > 4)
    {
        std::cout << "Usage: <main_path> <mode> [save_path]" << std::endl;
        return 1;
    }

    auto inPath = std::string(argv[1]);
    std::string outPath;
    if (argc == 4)
    {
        outPath = std::string(argv[3]);
    }

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
        return 1; // Exit on invalid mode
    }

    // Check for g++ using the OS-specific NULL_DEVICE
    if (mode != TRANSLATE && std::system(("g++ --version " + NULL_DEVICE).c_str()) != 0)
    {
        std::cerr << "g++ not installed" << std::endl;
        return -1;
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

    // Handle executable extension cross-platform
    std::filesystem::path exePath = outPath;
    if (!EXE_EXT.empty())
    {
        exePath.replace_extension(EXE_EXT);
    }
    else
    {
        // On Linux, ensure we remove any existing extension or keep it extension-less
        exePath.replace_extension("");
    }

    if (mode == COMPILE || mode == RUN)
    {
        std::ostringstream cmd;

        std::string filesStr;
        const auto paths = FileGraph::getAllCppPaths();

        for (const auto& path : paths)
        {
            filesStr += " \"" + path.string() + "\" ";
        }

        cmd << "g++ -pthread " << filesStr << "-Iincludes -o \"" << exePath.string() << "\"";

        #ifdef _WIN32
             cmd << " -static -static-libgcc -static-libstdc++";
        #endif

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

        #ifdef _WIN32
            // Windows: Open new window and pause
            cmd << "start \"\" cmd /c \"" << exePath.string() << " & pause\"";
        #else
            // Linux: Run in current terminal
            // We need ./ if it's in the current directory, but absolute path is safer
            if (exePath.is_absolute())
            {
                 cmd << "\"" << exePath.string() << "\"";
            }
            else
            {
                 cmd << "./\"" << exePath.string() << "\"";
            }
        #endif

        std::system(cmd.str().c_str());
    }

    return 0;
}