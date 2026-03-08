#pragma once
#include <string>
#include <vector>
#include <boost/regex.hpp>
#include "Token.h"
#include "TrieTree/TrieNode.h"

class Tokenizer
{
private:
	static boost::regex tokenRegex;
	static const std::vector<std::string> captureBlocks;
	std::unique_ptr<TrieNode> trieTree;


	void initTrieTree() const;
	std::vector<Token> tokenizeByTrieTree(const std::string& code, const std::filesystem::path& path);
	static bool checkBoundary(const std::string &code, const KeywordInfo *keyword, const size_t start, const size_t end);
	static size_t handleKeywordMatch(const std::string &code, const size_t row, const size_t col, const TokenType tokenType, std::vector<Token> &tokens, const size_t keywordEnd, const std::filesystem::path& path);
public:
	Tokenizer();
	std::vector<Token> tokenize(const std::string& code, const std::filesystem::path& path);
};
