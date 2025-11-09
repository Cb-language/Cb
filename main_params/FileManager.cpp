#include "FileManager.h"

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
		outPath = inPath.substr(0, length - 4) + ".cpp";
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
	std::string content;

	if(inputFile.is_open() == false)
	{
		return content;
	}
	std::string line;
	while (std::getline(inputFile, line))
	{
		content += line + "\n";
	}
	return content;
}

bool FileManager::writeFile(const std::string& data)
{
	if(outputFile.is_open() == false)
	{
		return false;
	}
	outputFile << data;
	outputFile.close();
	return true;
}
