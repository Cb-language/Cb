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
	std::unique_ptr<TrieNode> trieTree;


	void initTrieTree() const;
	std::vector<Token> tokenizeByTrieTree(const std::wstring& code, const std::filesystem::path& path);
public:
	Tokenizer();
	std::vector<Token> tokenize(const std::wstring& code, const std::filesystem::path& path);
};
