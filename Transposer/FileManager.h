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
private:
	std::wifstream inputFile;
	std::ofstream outputFile;
};