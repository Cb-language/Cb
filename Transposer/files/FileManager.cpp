#include "FileManager.h"

#include <algorithm>
#include <codecvt>
#include <sstream>

#include "ArrayHelper.h"
#include "../other/Utils.h"

bool FileManager::wroteArrayFiles = false;
std::string FileManager::currFolder = "C:\\";

FileManager::FileManager(const std::string& inPath, const std::string& outPath) : isOpen(false)
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

		this->inputPath = inPath;
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

	this->outputPath = outPath;

	if (inPath.find_last_of("/\\") != std::string::npos)
	{
		currFolder = inPath.substr(0, inPath.find_last_of("/\\"));
		std::ranges::replace(currFolder, '\\', '/');
	}

	isOpen = true;
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

	Utils::logMsg("Wrote to file successfully");
	return true;
}

std::string FileManager::getInputPath() const
{
	return inputPath;
}

std::string FileManager::getOutputPath() const
{
	return outputPath;
}

#include <filesystem>

bool FileManager::writeArrayFiles()
{
	if (wroteArrayFiles)
	{
		return true;
	}

	wroteArrayFiles = true;

	const std::filesystem::path includeDir = std::filesystem::path(currFolder) / "includes";

	std::filesystem::create_directories(includeDir);

	const std::filesystem::path hPath = includeDir / "Array.h";
	const std::filesystem::path tppPath = includeDir / "Array.tpp";

	std::ofstream hFile(hPath);
	if (!hFile)
	{
		std::cout << "Failed to write Array.h: " << hPath << std::endl;
		return false;
	}

	hFile << ArrayHelper::hStr;

	std::ofstream tppFile(tppPath);
	if (!tppFile)
	{
		std::cout << "Failed to write Array.tpp: " << tppPath << std::endl;
		return false;
	}

	tppFile << ArrayHelper::tppStr;

	return true;
}


bool FileManager::getIsOpen() const
{
	return isOpen;
}

std::string FileManager::getCurrFolder()
{
	return currFolder;
}

std::vector<std::filesystem::path> FileManager::getAllCppFiles()
{
	const std::filesystem::path rootDir = currFolder;
	std::vector<std::filesystem::path> result;

	if (!std::filesystem::exists(rootDir))
	{
		return result;
	}

	for (const auto& entry : std::filesystem::recursive_directory_iterator(rootDir))
	{
		if (!entry.is_regular_file())
		{
			continue;
		}

		if (entry.path().extension() == ".cpp")
		{
			result.push_back(entry.path());
		}
	}

	return result;
}
