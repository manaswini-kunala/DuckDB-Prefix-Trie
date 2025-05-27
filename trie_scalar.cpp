#include "duckdb.hpp"
#include "trie.hpp"

using namespace duckdb;

// Global trie instance
// static Trie trie;
extern Trie trie;

void TrieAddFun(DataChunk &args, ExpressionState &, Vector &result) {
    auto input = args.data[0];
    auto result_data = FlatVector::GetData<string_t>(result);
    
    for (idx_t i = 0; i < args.size(); i++) {
        auto word = args.GetValue(0, i).ToString();
        result_data[i] = StringVector::AddString(result, "OK");
        trie.AddWord(word);
    }
}

void TrieSearchFun(DataChunk &args, ExpressionState &, Vector &result) {
    auto input = args.data[0];
    auto result_data = FlatVector::GetData<bool>(result);

    for (idx_t i = 0; i < args.size(); i++) {
        auto word = args.GetValue(0, i).ToString();
        result_data[i] = trie.SearchWord(word);
    }
}

void TrieDeleteFun(DataChunk &args, ExpressionState &, Vector &result) {
    auto input = args.data[0];
    auto result_data = FlatVector::GetData<string_t>(result);

    for (idx_t i = 0; i < args.size(); i++) {
        auto word = args.GetValue(0, i).ToString();
        trie.Delete(word);
        result_data[i] = StringVector::AddString(result, "Deleted");
    }
}