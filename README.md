# 🧠 Trie-Backed Table Functions for DuckDB

This project adds two custom table functions to DuckDB powered by a **Trie (prefix tree)**. It enables efficient **prefix** and **wildcard** string searches over a set of in-memory words, making it ideal for text-heavy queries.

## 🚀 Features

- 🔍 **Prefix Search**: Find all words that begin with a given prefix.
- ✨ **Wildcard Search**: Support for `*` as a wildcard character matching zero or more letters.
- ⚡ **High Performance**: All matches are precomputed during the bind phase for fast execution.
- 📦 **Vectorized Streaming**: Results are streamed in batches using DuckDB's `DataChunk` interface.

## 📁 Project Structure

trie/ # Trie data structure implementation
├── trie.hpp
├── trie.cpp

duckdb_extension/ # DuckDB table functions
├── trie_prefix.cpp # Implements trie_prefix() table function
├── trie_wildcard.cpp # Implements trie_wildcard() table function

CMakeLists.txt # Build script for DuckDB extension
README.md # Project documentation

## 🧠 How It Works

- During the **bind phase**, the trie is traversed to collect all matches based on the input pattern.
- The results are stored in a buffer (`results[]`) and served in chunks during the **execution phase**.
- The implementation avoids any use of insertion-order tracking — all words are discovered dynamically through real-time traversal using depth-first search (DFS) or backtracking.

## 📊 Performance Insights

When working with **Zipfian-distributed data**, this trie structure performs especially well.  
Common prefixes stay close to the root of the tree, making lookups for those hot prefixes significantly faster.  
In practice, this leads to more than **2× speedup** for frequent patterns across datasets with tens of thousands of entries.

