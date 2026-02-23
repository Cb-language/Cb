#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

class LibHelper
{
public:
	static void write(const std::filesystem::path& dir);

	const static std::vector<std::pair<std::filesystem::path, std::string>> files;
};
