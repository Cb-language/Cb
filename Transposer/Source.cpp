#include <iostream>
#include <map>
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
    }

    if (!parser.checkLegal())
    {
        std::cerr << "Code Illegal :(" << std::endl;
    }

    fileManager.writeFile(parser.translateToCpp());


    return 0;
}
