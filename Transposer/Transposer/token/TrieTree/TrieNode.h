#pragma once
#include <optional>
#include <unordered_map>

#include "KeywordInfo.h"

class TrieNode
{
private:
    std::pmr::unordered_map<char, TrieNode*> children;
    const std::optional<KeywordInfo> keyword;
public:
    TrieNode();

    std::optional<KeywordInfo> getKeyword() const;
    void setKeyword(std::optional<KeywordInfo> keyword);
    void getOrCreateCild(const char& str);

    const std::optional<std::reference_wrapper<KeywordInfo>> getChild(const std::string& str) const;
};
