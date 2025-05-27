#pragma once
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/function/table_function.hpp"

// Declare your scalar function interfaces
void TrieAddFun(duckdb::DataChunk &args, duckdb::ExpressionState &, duckdb::Vector &result);
void TrieSearchFun(duckdb::DataChunk &args, duckdb::ExpressionState &, duckdb::Vector &result);
void TrieDeleteFun(duckdb::DataChunk &args, duckdb::ExpressionState &, duckdb::Vector &result);

namespace TriePrefixFunction {
    duckdb::unique_ptr<duckdb::FunctionData> Bind(
        duckdb::ClientContext &,
        duckdb::TableFunctionBindInput &,
        duckdb::vector<duckdb::LogicalType> &,
        duckdb::vector<std::string> &
    );
    void Function(duckdb::ClientContext &, duckdb::TableFunctionInput &, duckdb::DataChunk &);
}

namespace TrieWildcardFunction {
    duckdb::unique_ptr<duckdb::FunctionData> Bind(
        duckdb::ClientContext &,
        duckdb::TableFunctionBindInput &,
        duckdb::vector<duckdb::LogicalType> &,
        duckdb::vector<std::string> &
    );
    void Function(duckdb::ClientContext &, duckdb::TableFunctionInput &, duckdb::DataChunk &);
}