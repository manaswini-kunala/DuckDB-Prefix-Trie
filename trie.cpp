// #include "trie.hpp"
// #include <string>
// #include <vector>
// #include <iostream>
// // Constructor
// Trie::Trie() {
//     root = new TrieNode();
// }

// void Trie::AddWord(const std::string &word) {
//     if (word.empty()) return;
//     TrieNode* node = root;
//     for (char c : word) {
//         if (!node->children.count(c)) {
//             node->children[c] = new TrieNode();
//         }
//         node = node->children[c];
//     }
    
//     node->is_end = true;
// }

// bool Trie::SearchWord(const std::string &word) {
//     TrieNode* node = root;
//     for (char c : word) {
//         if (!node->children.count(c)) return false;
//         node = node->children[c];
//     }
//     return node->is_end;
// }

// bool Trie::DeleteWord(TrieNode* node, const std::string &word, int depth) {
//     if (!node) return false;

//     if (depth == word.size()) {
//         if (!node->is_end) return false;
//         node->is_end = false;
//         return node->children.empty();
//     }

//     char c = word[depth];
//     if (!node->children.count(c)) return false;

//     bool shouldDeleteChild = DeleteWord(node->children[c], word, depth + 1);

//     if (shouldDeleteChild) {
//         delete node->children[c];
//         node->children.erase(c);
//         return !node->is_end && node->children.empty();
//     }

//     return false;
// }


// void Trie::PrintTrieStructure() {
//     std::cerr << "[DEBUG] Starting Trie Printout:" << std::endl;
//     PrintNode(root, "");
// }

// void Trie::PrintNode(TrieNode* node, const std::string &indent) {
//     if (node->is_end) {
//         std::cerr << indent << "(word end)" << std::endl;
//     }

//     for (auto &pair : node->children) {
//         std::cerr << indent << "- " << pair.first << std::endl;
//         PrintNode(pair.second, indent + "  ");
//     }
// }

// void Trie::Delete(const std::string &word) {
//     DeleteWord(root, word, 0);
// }

// void Trie::PrefixDFS(TrieNode* node, std::string current, std::vector<std::string>& result, int depth) {
//     if (!node) {
//         std::cerr << "[DEBUG] Null node reached at depth " << depth << std::endl;
//         return;
//     }

//     std::cerr << "[DEBUG] DFS at depth " << depth << " with current string: " << current << std::endl;

//     if (node->is_end) {
//         std::cerr << "[DEBUG] Word found: " << current << std::endl;
//         result.push_back(current);
//     }

//     for (auto &pair : node->children) {
//         std::cerr << "[DEBUG] Traversing child: " << pair.first << " from string: " << current << std::endl;
//         PrefixDFS(pair.second, current + pair.first, result, depth + 1);
//     }
// }

// void Trie::PrefixSearch(const std::string &prefix, std::vector<std::string>& result) {
//     std::cerr << "[DEBUG] PrefixSearch called with prefix: " << prefix << std::endl;

//     TrieNode* node = root;
//     for (char c : prefix) {
//         std::cerr << "[DEBUG] Traversing char: " << c << std::endl;

//         if (!node->children.count(c)) {
//             std::cerr << "[DEBUG] Character not found in children: " << c << std::endl;
//             return;
//         }

//         node = node->children[c];
//     }

//     std::cerr << "[DEBUG] Prefix path found, starting DFS..." << std::endl;
//     PrefixDFS(node, prefix, result, 0);
// }

// void Trie::WildcardDFS(TrieNode* node, const std::string &pattern, int depth, std::string current, std::vector<std::string>& result) {
//     if (!node) return;

//     if (depth == pattern.size()) {
//         if (node->is_end) result.push_back(current);
//         return;
//     }

//     char c = pattern[depth];
//     if (c == '*') {
//         for (auto &pair : node->children) {
//             WildcardDFS(pair.second, pattern, depth + 1, current + pair.first, result);
//         }
//     } else {
//         if (node->children.count(c)) {
//             WildcardDFS(node->children[c], pattern, depth + 1, current + c, result);
//         }
//     }
// }

// void Trie::WildcardSearch(const std::string &pattern, std::vector<std::string>& result) {
//     WildcardDFS(root, pattern, 0, "", result);
// }

// // ✅ Global instance — used by all functions
// Trie trie;

#include "trie.hpp"
#include <string>
#include <vector>
#include <iostream>

// Constructor
Trie::Trie() {
    root = new TrieNode();
}

void Trie::AddWord(const std::string &word) {
    if (word.empty()) return;
    TrieNode* node = root;
    for (char c : word) {
        if (!node->children.count(c)) {
            node->children[c] = new TrieNode();
        }
        node = node->children[c];
    }
    // Only record and mark end if this is a new word
    if (!node->is_end) {
        node->is_end = true;
        // Preserve insertion order for deterministic results
        insertion_order.push_back(word);
    }
}

bool Trie::SearchWord(const std::string &word) {
    TrieNode* node = root;
    for (char c : word) {
        if (!node->children.count(c)) return false;
        node = node->children[c];
    }
    return node->is_end;
}

bool Trie::DeleteWord(TrieNode* node, const std::string &word, int depth) {
    if (!node) return false;

    if (depth == word.size()) {
        if (!node->is_end) return false;
        node->is_end = false;
        return node->children.empty();
    }

    char c = word[depth];
    if (!node->children.count(c)) return false;

    bool shouldDeleteChild = DeleteWord(node->children[c], word, depth + 1);

    if (shouldDeleteChild) {
        delete node->children[c];
        node->children.erase(c);
        return !node->is_end && node->children.empty();
    }

    return false;
}

void Trie::PrintTrieStructure() {
    std::cerr << "[DEBUG] Starting Trie Printout:" << std::endl;
    PrintNode(root, "");
}

void Trie::PrintNode(TrieNode* node, const std::string &indent) {
    if (node->is_end) {
        std::cerr << indent << "(word end)" << std::endl;
    }

    for (auto &pair : node->children) {
        std::cerr << indent << "- " << pair.first << std::endl;
        PrintNode(pair.second, indent + "  ");
    }
}

void Trie::Delete(const std::string &word) {
    DeleteWord(root, word, 0);
}

void Trie::PrefixDFS(TrieNode* node, std::string current, std::vector<std::string>& result, int depth) {
    if (!node) return;
    if (node->is_end) {
        result.push_back(current);
    }
    for (auto &pair : node->children) {
        PrefixDFS(pair.second, current + pair.first, result, depth + 1);
    }
}

void Trie::PrefixSearch(const std::string &prefix, std::vector<std::string>& result) {
    TrieNode* node = root;
    for (char c : prefix) {
        if (!node->children.count(c)) return;
        node = node->children[c];
    }
    PrefixDFS(node, prefix, result, 0);
}

void Trie::WildcardDFS(TrieNode* node, const std::string &pattern, int depth, std::string current, std::vector<std::string>& result) {
    if (!node) return;
    if (depth == pattern.size()) {
        if (node->is_end) result.push_back(current);
        return;
    }
    char c = pattern[depth];
    if (c == '*') {
        for (auto &pair : node->children) {
            WildcardDFS(pair.second, pattern, depth + 1, current + pair.first, result);
        }
    } else {
        if (node->children.count(c)) {
            WildcardDFS(node->children[c], pattern, depth + 1, current + c, result);
        }
    }
}

void Trie::WildcardSearch(const std::string &pattern, std::vector<std::string>& result) {
    WildcardDFS(root, pattern, 0, "", result);
}

// Global instance — used by all functions
Trie trie;

