#pragma once

#include <iostream>
#include <string>
#include <fstream>

class FileManager
{
public:
	FileManager(const std::string& inPath, const std::string& outPath);
	std::wstring readFile();
	bool writeFile(const std::string& data);

	std::string getInputPath() const;
	std::string getOutputPath() const;
private:
	std::wifstream inputFile;
	std::ofstream outputFile;

	std::string inputPath;
	std::string outputPath;
};