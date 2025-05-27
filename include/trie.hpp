// #pragma once

// #include <unordered_map>
// #include <vector>
// #include <string>

// struct TrieNode {
//     std::unordered_map<char, TrieNode*> children;
//     bool is_end = false;
// };

// class Trie {
// public:
//     TrieNode* root;
//     Trie();

//     void AddWord(const std::string &word);
//     bool SearchWord(const std::string &word);
//     void Delete(const std::string &word);

//     void PrefixSearch(const std::string &prefix, std::vector<std::string>& result);
//     void WildcardSearch(const std::string &pattern, std::vector<std::string>& result);

//     // ✅ Debug utility to print the entire Trie structure
//     void PrintTrieStructure();

// private:
//     bool DeleteWord(TrieNode* node, const std::string &word, int depth);

//     void PrefixDFS(TrieNode* node, std::string current, std::vector<std::string>& result, int depth);
//     void WildcardDFS(TrieNode* node, const std::string &pattern, int depth, std::string current, std::vector<std::string>& result);

//     // 🔧 Internal recursive helper for printing
//     void PrintNode(TrieNode* node, const std::string &indent);
// };


#pragma once

#include <unordered_map>
#include <vector>
#include <string>

struct TrieNode {
    std::unordered_map<char, TrieNode*> children;
    bool is_end = false;
};

class Trie {
public:
    TrieNode* root;
    // Maintain insertion order of words for ordered prefix/wildcard searches
    std::vector<std::string> insertion_order;

    Trie();

    // Add a word to the trie; record it if newly inserted
    void AddWord(const std::string &word);
    bool SearchWord(const std::string &word);
    void Delete(const std::string &word);

    // Ordered searches based on insertion_order
    void PrefixSearch(const std::string &prefix, std::vector<std::string>& result);
    void WildcardSearch(const std::string &pattern, std::vector<std::string>& result);

    // Debug utility to print the entire Trie structure
    void PrintTrieStructure();

private:
    bool DeleteWord(TrieNode* node, const std::string &word, int depth);

    void PrefixDFS(TrieNode* node, std::string current, std::vector<std::string>& result, int depth);
    void WildcardDFS(TrieNode* node, const std::string &pattern, int depth, std::string current, std::vector<std::string>& result);

    // Internal recursive helper for printing
    void PrintNode(TrieNode* node, const std::string &indent);
};
