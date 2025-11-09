#include "FileManager.h"
#include <sstream>

FileManager::FileManager(const std::string& inPath, std::string& outPath)
{
	int length = inPath.length();

	// Check for .cb extension in sorce file
	if (inPath[length - 3] == '.' && inPath[length - 2] == 'c' && inPath[length - 1] == 'b')
	{
		inputFile.open(inPath);
		if (!inputFile.is_open())
		{
			std::cout << "Failed to open input file: " << inPath << std::endl;
			return;
		}
	}

	// If no output file path provided, create one by replacing .cb with .cpp
	if (outPath.empty())
	{
		outPath = inPath.substr(0, length - 3) + ".cpp";
	}

	// Open output file in write mode
	outputFile.open(outPath);
	if (!outputFile.is_open())
	{
		std::cout << "Failed to open output file: " << outPath << std::endl;
		return;
	}
}


std::string FileManager::readFile()
{
	std::ostringstream ss;

	if (inputFile.is_open() == false)
	{
		return "";
	}

	ss << inputFile.rdbuf();
	return ss.str();
}

bool FileManager::writeFile(const std::string& data)
{
	if (outputFile.is_open() == false)
	{
		return false;
	}
	outputFile << data;
	outputFile.close();
	return true;
}
