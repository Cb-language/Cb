#include <iostream>
#include <map>
#include <sstream>
#include <windows.h>
#include "token/Tokenizer.h"
#include "FileManager.h"
#include "parser/Parser.h"

enum Mode
{
    TRANSLATE,
    COMPILE,
    RUN
};

int main(int argc, char* argv[])
{
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
            std::cout <<  "Translating Mode" << std::endl;
        break;
    case RUN:
            std::cout << "Running Mode" << std::endl;
        break;
    case COMPILE:
        std::cout << "Compiling Mode" << std::endl;
    break;
    default:
        std::cout << "Invalid mode. Use T for Translate, C for Compile, R for Run." << std::endl;
    }

    if (mode != TRANSLATE && std::system("g++ --version >nul 2>&1") != 0)
    {
        std::cerr << "g++ not installed" << std::endl;
    }

    FileManager fileManager(inPath, outPath);

    const std::wstring wstr = fileManager.readFile();
    Parser parser(Tokenizer::tokenize(wstr));

    try
    {
        parser.parse();
    }
    catch (const Error& err)
    {
        std::cerr << err.what() << std::endl;
        return -1;
    }

    if (!parser.checkLegal())
    {
        std::cerr << "Code Illegal :(" << std::endl;
        return -2;
    }

    fileManager.writeFile(parser.translateToCpp());


    std::string exePath = fileManager.getOutputPath();

    if (mode == COMPILE || mode == RUN)
    {
        std::ostringstream cmd;
        size_t pos = exePath.rfind(".cpp");
        if (pos != std::string::npos)
        {
            exePath.replace(pos, 4, ".exe");
        }
        else
        {
            std::cerr << "Error with output path :(" << std::endl;
            return 2;
        }
        cmd << "g++ \"" << fileManager.getOutputPath() << "\" -o \"" << exePath << "\"" << std::endl;

        std::system(cmd.str().c_str());
    }

    if (mode == RUN)
    {
        exePath = "\"" + exePath + "\"";
        std::system(exePath.c_str());
    }

    return 0;
}
