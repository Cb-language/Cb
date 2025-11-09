#include <iostream>
#include <map>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include "Tokenizer.h"
#include "FileManager.h"

enum Mode
{
    TRANSLATE,
    COMPILE,
    RUN
};


// Helper function to print a token using WriteConsoleW
void printToken(const Token& tok, const std::map<Token::TokenType, std::wstring>& tokenTypeNames)
{
    std::wstring output =
        L"Line: " + std::to_wstring(tok.line) +
        L", Column: " + std::to_wstring(tok.column) +
        L", Value: \"" + tok.value + L"\"" +
        L", Type: " + tokenTypeNames.at(tok.type) + L"\n";

    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), output.size(), nullptr, nullptr);
}

int main(int argc, char* argv[])
{
    // Enable UTF-8 output (mostly for other parts that may use cout)
    SetConsoleOutputCP(CP_UTF8);
    //_setmode(_fileno(stdout), _O_U8TEXT);

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
    FileManager fileManager(inPath, outPath);

    std::string content = fileManager.readFile();
    fileManager.writeFile(content);

    // Determine mode of operation
    int mode = argv[2] == "T" ? TRANSLATE : (argv[2] == "R" ? RUN : (argv[2] == "C" ? COMPILE : -1));
    switch (mode)
    {
    case TRANSLATE:
        break;
    case RUN:
        break;
    case COMPILE:
        break;
    default:
        std::cout << "Invalid mode. Use T for Translate, C for Compile, R for Run." << std::endl;
    }

    // Print for verification
    std::cout << content << std::endl;

    // Initialize tokenizer
    Tokenizer::init();

    std::cout << "Note: some chars used in out lang are double wchar, so we cant print them" << std::endl << std::endl;

    // Map TokenType enum to readable string
    std::map<Token::TokenType, std::wstring> tokenTypeNames = {
        {Token::IDENTIFIAR, L"IDENTIFIAR"},
        {Token::TYPE, L"TYPE"},
        {Token::KEYWORD, L"KEYWORD"},
        {Token::PUNCTUATION, L"PUNCTUATION"},
        {Token::OP_UNARY, L"OP_UNARY"},
        {Token::OP_BINARY, L"OP_BINARY"},
        {Token::OP_ASSIGNMENT, L"OP_ASSIGNMENT"},
        {Token::CONST_INT, L"CONST_INT"},
        {Token::CONST_FLOAT, L"CONST_FLOAT"},
        {Token::CONST_CHAR, L"CONST_CHAR"},
        {Token::CONST_BOOL, L"CONST_BOOL"},
        {Token::CONST_STR, L"CONST_STR"},
        {Token::COMMENT_SINGLE, L"COMMENT_SINGLE"},
        {Token::COMMENT_MULTI, L"COMMENT_MULTI"}
    };

    // Example code to tokenize
    const std::wstring code = LR"(
? This is a single-line comment
?* This is a 
multi-line comment *?
cres demen .14 42 'a' "hello
world"
flat degree playBar hear D Fmin
♯ ♭ ♮ ☉ ∮ ,
x += 5
y == 10
z // 2
[] ║ \ identifier_name ∮
=! != <= > < //= ()║::║:║║:║
)";

    // Tokenize
    std::vector<Token> tokens = Tokenizer::tokenize(code);

    // Print tokens
    for (const auto& tok : tokens)
        printToken(tok, tokenTypeNames);

    return 0;
}
