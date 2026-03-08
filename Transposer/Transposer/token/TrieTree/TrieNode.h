#pragma once
#include <optional>
#include <unordered_map>

#include "KeywordInfo.h"

class TrieNode
{
private:
    std::pmr::unordered_map<char, std::unique_ptr<TrieNode>> children;
    std::optional<std::reference_wrapper<const KeywordInfo>> keyword;
public:
    TrieNode();

    std::optional<std::reference_wrapper<const KeywordInfo>> getKeyword() const;
    void setKeyword(const KeywordInfo& keyword);
    TrieNode& getOrCreateCild(char c);

    std::optional<std::reference_wrapper<TrieNode>> getChild(const char c) const;
};
