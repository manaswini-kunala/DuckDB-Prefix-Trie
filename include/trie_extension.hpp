// #pragma once

// #include "duckdb.hpp"

// namespace duckdb {

// class TrieExtension : public Extension {
// public:
// 	void Load(DuckDB &db) override;
// 	std::string Name() override;
//         std::string Version() const override;
// };

// } // namespace duckdb

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

// private:
//     bool DeleteWord(TrieNode* node, const std::string &word, int depth);
//     void PrefixDFS(TrieNode* node, std::string current, std::vector<std::string>& result);
//     void WildcardDFS(TrieNode* node, const std::string &pattern, int depth, std::string current, std::vector<std::string>& result);
// };

#pragma once

#include "duckdb.hpp"

namespace duckdb {

class TrieExtension : public Extension {
public:
    void Load(DuckDB &db) override;
    std::string Name() override;
    std::string Version() const override;
};

}
