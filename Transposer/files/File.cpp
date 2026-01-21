#include "File.h"

#include <fstream>

#include "errorHandling/Error.h"
#include "token/Tokenizer.h"

std::filesystem::path File::mainPath = "C\\:";
std::filesystem::path File::outDir = "C\\:";

File::File(const std::wstring& inFilename, const std::wstring& outFilename)
    : inPath(mainPath / inFilename), outPath(outDir / outFilename), parser(Parser(tokenize()))
{
}

File::File(const std::filesystem::path& path) : File(path, std::filesystem::path(path).replace_extension("cpp"))
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

const std::vector<std::pair<std::filesystem::path, Token>>& File::getIncludes()
{
    if (readIncludes) return includes;
    readIncludes = true;

    includes = std::vector(parser.readIncludes());
    return includes;
}

void File::parse()
{
    if (parsed) return;
    parsed = true;
    parser.parse();
}

void File::analyze()
{
    if (analyzed) return;
    if (!parsed) parse();
    analyzed = true;
    parser.analyze();
}

void File::write() const
{
    std::ofstream file(outPath, std::ios::out);

    if(!file || !file.is_open())
    {
        throw Error(Token(Token::COMMENT_SINGLE, inPath.wstring(),0,0),
            "Unexpected Error: couldn't open file: \"" + inPath.string() + "\"");
    }
    file << parser.translateToCpp();

    file.close();
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

    const std::wstring data = std::wstring((std::istreambuf_iterator<wchar_t>(file)),
                             std::istreambuf_iterator<wchar_t>());

    file.close();

    return Tokenizer::tokenize(data);
}

void File::setMainPath(const std::filesystem::path& mainPath)
{
    File::mainPath = mainPath;
}

const std::filesystem::path& File::getMainPath()
{
    return mainPath;
}

void File::setOutDir(const std::filesystem::path& outDir)
{
    File::outDir = outDir;
}

const std::filesystem::path& File::getOutDir()
{
    return outDir;
}
