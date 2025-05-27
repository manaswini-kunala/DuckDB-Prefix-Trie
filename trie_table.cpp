
#include "duckdb.hpp"
#include "trie.hpp"
#include <iostream>
#include <vector>
#include <string>
using namespace duckdb;

extern Trie trie;

namespace TriePrefixFunction {

    // Store prefix, full results, and an offset
    struct PrefixBindData : public FunctionData {
        string prefix;
        vector<string> results;
        mutable idx_t offset = 0;

        explicit PrefixBindData(string p) : prefix(std::move(p)) {
            // Build results in insertion order for deterministic output
            for (const auto &w : trie.insertion_order) {
                if (w.rfind(prefix, 0) == 0) { // starts with prefix
                    results.push_back(w);
                }
            }
        }

        unique_ptr<FunctionData> Copy() const override {
            auto raw_copy = make_uniq<PrefixBindData>(prefix);
            raw_copy->results = results;
            raw_copy->offset  = offset;
            return unique_ptr<FunctionData>(raw_copy.release());
        }

        bool Equals(const FunctionData &other) const override {
            auto &o = other.Cast<PrefixBindData>();
            return prefix == o.prefix
                && results == o.results
                && offset  == o.offset;
        }
    };

    // Bind: prepare return types, names, and bind data
    unique_ptr<FunctionData> Bind(ClientContext &, TableFunctionBindInput &input,
                                  vector<LogicalType> &return_types, vector<string> &names) {
        string prefix = input.inputs[0].ToString();
        return_types.push_back(LogicalType::VARCHAR);
        names.push_back("word");
        return make_uniq<PrefixBindData>(prefix);
    }

    // Function: page through results and signal end-of-stream
    void Function(ClientContext &, TableFunctionInput &input, DataChunk &output) {
        auto &info = input.bind_data->Cast<PrefixBindData>();

        // If all rows have been returned, emit empty chunk
        if (info.offset >= info.results.size()) {
            output.SetCardinality(0);
            return;
        }

        // Determine chunk size (all remaining for simplicity)
        idx_t remaining = info.results.size() - info.offset;
        idx_t chunk_size = remaining;

        auto out_data = FlatVector::GetData<string_t>(output.data[0]);
        for (idx_t i = 0; i < chunk_size; i++) {
            out_data[i] = StringVector::AddString(
                output.data[0], info.results[info.offset + i]
            );
        }
        output.SetCardinality(chunk_size);

        // Advance offset for next call
        info.offset += chunk_size;
    }

} // namespace TriePrefixFunction

namespace TrieWildcardFunction {

    // Store pattern, full results, and an offset
    struct WildcardBindData : public FunctionData {
        string pattern;
        vector<string> results;
        mutable idx_t offset = 0;

        explicit WildcardBindData(string p) : pattern(std::move(p)) {
            // Build results in insertion order for deterministic output
            for (const auto &w : trie.insertion_order) {
                if (w.size() == pattern.size()) {
                    bool match = true;
                    for (size_t i = 0; i < pattern.size(); i++) {
                        if (pattern[i] != '*' && pattern[i] != w[i]) {
                            match = false;
                            break;
                        }
                    }
                    if (match) {
                        results.push_back(w);
                    }
                }
            }
        }

        unique_ptr<FunctionData> Copy() const override {
            auto raw_copy = make_uniq<WildcardBindData>(pattern);
            raw_copy->results = results;
            raw_copy->offset  = offset;
            return unique_ptr<FunctionData>(raw_copy.release());
        }

        bool Equals(const FunctionData &other) const override {
            auto &o = other.Cast<WildcardBindData>();
            return pattern == o.pattern
                && results == o.results
                && offset  == o.offset;
        }
    };

    // Bind: prepare return types, names, and bind data
    unique_ptr<FunctionData> Bind(ClientContext &, TableFunctionBindInput &input,
                                  vector<LogicalType> &return_types, vector<string> &names) {
        string pattern = input.inputs[0].ToString();
        return_types.push_back(LogicalType::VARCHAR);
        names.push_back("match");
        return make_uniq<WildcardBindData>(pattern);
    }

    // Function: page through results and signal end-of-stream
    void Function(ClientContext &, TableFunctionInput &input, DataChunk &output) {
        auto &info = input.bind_data->Cast<WildcardBindData>();

        // If all rows have been returned, emit empty chunk
        if (info.offset >= info.results.size()) {
            output.SetCardinality(0);
            return;
        }

        idx_t remaining = info.results.size() - info.offset;
        idx_t chunk_size = remaining;

        auto out_data = FlatVector::GetData<string_t>(output.data[0]);
        for (idx_t i = 0; i < chunk_size; i++) {
            out_data[i] = StringVector::AddString(
                output.data[0], info.results[info.offset + i]
            );
        }
        output.SetCardinality(chunk_size);

        // Advance offset for next call
        info.offset += chunk_size;
    }

} // namespace TrieWildcardFunction
