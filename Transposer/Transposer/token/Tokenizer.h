#pragma once
#include <string>
#include <queue>
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
	static bool checkBoundary(const std::string &code, const KeywordInfo *keyword, const size_t start, const size_t end);
	static size_t handleKeywordMatch(const std::string &code, size_t& row, size_t& col, const CbTokenType tokenType, std::queue<Token>& tokens, const size_t keywordEnd, const size_t keywordStart, const std::filesystem::path& path);
	static void onRegexToken(Token *token);
public:
	Tokenizer();
	std::queue<Token> tokenize(const std::string& code, const std::filesystem::path& path) const;
};
