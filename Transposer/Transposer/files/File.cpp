#include "File.h"

#include <fstream>

#include "errorHandling/Error.h"
#include "errorHandling/preproccessorErrors/CouldntOpenFile.h"
#include "token/Tokenizer.h"

std::filesystem::path File::mainPath = "C\\:";
std::filesystem::path File::outDir = "C\\:";

File::File(const std::string& inFilename, const std::string& outFilename)
    : inPath(mainPath / inFilename), outPathH((outDir / outFilename).replace_extension("h")),
    outPathCpp((outDir / outFilename).replace_extension("cpp")), parser(Parser(tokenize()))
{
}

File::File(const std::filesystem::path& inPath, const std::filesystem::path& outPath)
    : inPath(mainPath / Utils::normalizePath(inPath)), outPathH((outDir / Utils::normalizePath(outPath)).replace_extension("h")),
        outPathCpp((outDir / Utils::normalizePath(outPath)).replace_extension("cpp")), parser(Parser(tokenize()))
{
}

File::File(const std::filesystem::path& path) : File(path, std::filesystem::path(path))
{
}

const std::filesystem::path& File::getInPath() const
{
    return inPath;
}

const std::filesystem::path& File::getOutHPath() const
{
    return outPathH;
}

const std::filesystem::path& File::getOutCppPath() const
{
    return outPathCpp;
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

void File::write(const bool isMain)
{
    if (writen) return;

    writen = true;

    if (!isMain)
    {
        std::ofstream fileH(outPathH, std::ios::out);

        if(!fileH || !fileH.is_open())
        {
            throw CouldntOpenFile(Token(TokenType::ERROR_TOKEN, inPath.string(),0,0, outPathH), outPathH);
        }
        fileH << parser.translateToH();

        fileH.close();
    }

    std::ofstream fileCpp(outPathCpp, std::ios::out);

    if(!fileCpp || !fileCpp.is_open())
    {
        throw CouldntOpenFile(Token(TokenType::ERROR_TOKEN, inPath.string(),0,0, outPathCpp), outPathCpp);
    }
    fileCpp << parser.translateToCpp(outPathH, isMain);

    fileCpp.close();
}

const std::vector<std::unique_ptr<Error>>& File::getErrors() const
{
    return parser.getContext().getErrors();
}

std::vector<Token> File::tokenize() const
{
    std::ifstream file(inPath, std::ios::binary);

    if(!file || !file.is_open())
    {
        throw CouldntOpenFile(Token(TokenType::COMMENT_SINGLE, inPath.string(),0,0, inPath), inPath);
    }

    const std::string data = std::string((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

    file.close();

    return tokenizer.tokenize(data, inPath);
}

void File::setMainPath(const std::filesystem::path& mainPath)
{
    File::mainPath = mainPath.parent_path();
}

const std::filesystem::path& File::getMainPath()
{
    return mainPath;
}

void File::setOutDir(const std::filesystem::path& outDir)
{
    File::outDir = outDir.parent_path();
}

const std::filesystem::path& File::getOutDir()
{
    return outDir;
}

Parser& File::getParser()
{
    return parser;
}
