#include "TrieNode.h"

TrieNode::TrieNode()
{
}

std::optional<std::reference_wrapper<const KeywordInfo>> TrieNode::getKeyword() const
{
    return this->keyword;
}

void TrieNode::setKeyword(const KeywordInfo& keyword)
{
    this->keyword = keyword;
}

TrieNode& TrieNode::getOrCreateCild(char c)
{
    if (!getChild(c).has_value()) // creation
    {
        return *this->children.emplace(c, std::make_unique<TrieNode>()).first->second;
    }
    return getChild(c).value().get();
}

std::optional<std::reference_wrapper<TrieNode>> TrieNode::getChild(const char c) const
{
    if (children.contains(c))
    {
        return *children.at(c);
    }
    return std::nullopt;

}
