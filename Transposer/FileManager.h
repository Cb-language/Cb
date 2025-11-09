#pragma once

#include <iostream>
#include <string>
#include <fstream>

class FileManager
{
public:
	FileManager(const std::string& inPath, std::string& outPath);
	std::string readFile();
	bool writeFile(const std::string& data);
private:
	std::ifstream inputFile;
	std::ofstream outputFile;
};