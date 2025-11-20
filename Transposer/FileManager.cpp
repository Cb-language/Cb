#include "FileManager.h"

#include <codecvt>
#include <sstream>

FileManager::FileManager(const std::string& inPath, const std::string& outPath)
{
	int length = inPath.length();

	// Check for .cb extension in source file
	if (inPath.ends_with(".cb"))
	{
		inputFile.open(inPath, std::ios::binary);
		if (!inputFile.is_open())
		{
			std::cout << "Failed to open input file: " << inPath << std::endl;
			return;
		}
		inputFile.imbue(std::locale(inputFile.getloc(), new std::codecvt_utf8<wchar_t>));
	}

	std::string outputPath = outPath;
	// If no output file path provided, create one by replacing .cb with .cpp
	if (outPath.empty())
	{
		outputPath = inPath.substr(0, length - 3) + ".cpp";
	}

	// Open output file in write mode
	outputFile.open(outputPath);
	if (!outputFile.is_open())
	{
		std::cout << "Failed to open output file: " << outPath << std::endl;
		return;
	}
}

std::wstring FileManager::readFile()
{
	if (inputFile.is_open() == false)
	{
		return L"";
	}

	// Reset stream
	inputFile.clear();
	inputFile.seekg(0);

	return std::wstring((std::istreambuf_iterator<wchar_t>(inputFile)),
							 std::istreambuf_iterator<wchar_t>());
}

bool FileManager::writeFile(const std::string& data)
{
	if (outputFile.is_open() == false)
	{
		return false;
	}

	outputFile << data;
	outputFile.close();

	std::cout << "Wrote to file succssesfully" << std::endl;
	return true;
}
