#include <iostream>
#include "FileManager.h"

enum Mode
{
	TRANSLATE,
	COMPILE,
	RUN
};

int main(int argc, char* argv[])
{
	// Check for correct number of arguments
	if(argc < 2)
	{
		std::cout << "Usage: <main_path> <mode> [save_path]" << std::endl;
		return 1;
	}

	// Create FileManager instance and perform file operations
	std::string inPath = std::string(argv[1]);
	std::string outPath;
	if (argc == 3)
	{
		std::string outPath = std::string(argv[3]);
	}
	FileManager fileManager(inPath, outPath);
	
	std::string content = fileManager.readFile();

	// Determine mode of operation
	int mode = argv[2] == "T" ? TRANSLATE : (argv[2] == "R" ? RUN : (argv[2] == "C" ? COMPILE : -1));
	switch (mode)
	{
		case TRANSLATE:
			break;
		case RUN:
			break;
		case COMPILE:
			break;
		default:
			std::cout << "Invalid mode. Use T for Translate, C for Compile, R for Run." << std::endl;
	}

	// Print for verification
	std::cout << content << std::endl;

	return 0;
}