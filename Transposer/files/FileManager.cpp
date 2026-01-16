#include "FileManager.h"

#include <algorithm>
#include <codecvt>
#include <sstream>

#include "ArrayHelper.h"
#include "../other/Utils.h"

bool FileManager::wroteArrayFiles = false;
std::filesystem::path FileManager::folder = "C:\\";

FileManager::FileManager(const std::string& inPath, const std::string& outPath) : isOpen(false)
{
	this->inputPath = inPath;
	this->outputPath = outPath;

	if (inputPath.parent_path() != outputPath.parent_path())
	{
		std::filesystem::create_directories(outputPath.parent_path());
	}

	// Check for .cb extension in source file
	if (inputPath.extension() == ".cb")
	{
		inputFile.open(inputPath, std::ios::binary);
		if (!inputFile.is_open())
		{
			std::cout << "Failed to open input file: " << inPath << std::endl;
			return;
		}
		inputFile.imbue(std::locale(inputFile.getloc(), new std::codecvt_utf8<wchar_t>));
	}

	// If no output file path provided, create one by replacing .cb with .cpp
	if (outPath.empty())
	{
		outputPath = inputPath;
		outputPath.replace_extension(".cpp");
	}

	// Open output file in write mode
	outputFile.open(outputPath);
	if (!outputFile.is_open())
	{
		std::cout << "Failed to open output file: " << outPath << std::endl;
		return;
	}

	folder = outputPath;
	folder = folder.parent_path();
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

std::filesystem::path FileManager::getInputPath() const
{
	return inputPath;
}

std::filesystem::path FileManager::getOutputPath() const
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

	const std::filesystem::path includeDir = std::filesystem::path(folder) / "includes";

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

std::filesystem::path FileManager::getCurrFolder()
{
	return folder;
}

std::vector<std::filesystem::path> FileManager::getAllCppFiles()
{
	const std::filesystem::path rootDir = folder;
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
