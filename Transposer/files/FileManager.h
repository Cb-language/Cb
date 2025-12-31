#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

class FileManager
{
public:
	FileManager(const std::string& inPath, const std::string& outPath);
	std::wstring readFile();
	bool writeFile(const std::string& data);

	std::filesystem::path getInputPath() const;
	std::filesystem::path getOutputPath() const;

	static bool writeArrayFiles();

	bool getIsOpen() const;

	static std::filesystem::path getCurrFolder();

	static std::vector<std::filesystem::path> getAllCppFiles();
private:
	std::wifstream inputFile;
	std::ofstream outputFile;

	std::filesystem::path inputPath;
	std::filesystem::path outputPath;

	bool isOpen;

	static bool wroteArrayFiles;
	static std::filesystem::path folder;
};
