#include "File.h"

#include <fstream>

#include "errorHandling/Error.h"
#include "token/Tokenizer.h"

std::filesystem::path File::mainPath = "C\\:";

File::File(const std::wstring& inPath, const std::wstring& outPath)
    : inPath(mainPath / inPath), outPath(mainPath / outPath), parser(Parser(tokenize()))
{
}

const std::filesystem::path& File::getInPath() const
{
    return inPath;
}

const std::filesystem::path& File::getOutPath() const
{
    return outPath;
}

std::vector<Token> File::tokenize() const
{
    std::wifstream file(inPath, std::ios::binary);

    if(!file || !file.is_open())
    {
        throw Error(Token(Token::COMMENT_SINGLE, inPath.wstring(),0,0),
            "Unexpected Error: couldn't open file: \"" + inPath.string() + "\"");
    }
    file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t>));

    file.clear();
    file.seekg(0);

    return Tokenizer::tokenize(std::wstring((std::istreambuf_iterator<wchar_t>(file)),
                             std::istreambuf_iterator<wchar_t>()));
}

void File::setMainPath(const std::filesystem::path& mainPath)
{
    File::mainPath = mainPath;
}
