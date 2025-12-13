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

	std::string getInputPath() const;
	std::string getOutputPath() const;

	static bool writeArrayFiles();

	bool getIsOpen() const;

	static std::string getCurrFolder();

	static std::vector<std::filesystem::path> getAllCppFiles();
private:
	std::wifstream inputFile;
	std::ofstream outputFile;

	std::string inputPath;
	std::string outputPath;

	bool isOpen;

	static bool wroteArrayFiles;
	static std::string currFolder;
};
