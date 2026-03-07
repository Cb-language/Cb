#pragma once
#include <string>
#include <vector>
#include <boost/regex.hpp>
#include "Token.h"
#include "TrieTree/TrieNode.h"

class Tokenizer
{
private:
	static boost::wregex tokenRegex;
	static const std::vector<std::wstring> captureBlocks;
	static bool inited;

	static std::vector<Token> clean(std::vector<Token>& tokens);

	std::unique_ptr<TrieNode> trieTree;
	void initTrieTree();
public:
	static void init();
	static std::vector<Token> tokenize(const std::wstring& code, const std::filesystem::path& path);
};
