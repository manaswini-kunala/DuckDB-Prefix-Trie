// // #define DUCKDB_EXTENSION_MAIN

// // #include "trie_extension.hpp"
// // #include "duckdb.hpp"
// // #include "duckdb/common/exception.hpp"
// // #include "duckdb/common/string_util.hpp"
// // #include "duckdb/function/scalar_function.hpp"
// // #include "duckdb/main/extension_util.hpp"
// // #include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>

// // // OpenSSL linked through vcpkg
// // #include <openssl/opensslv.h>

// // namespace duckdb {

// // inline void TrieScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
// //     auto &name_vector = args.data[0];
// //     UnaryExecutor::Execute<string_t, string_t>(
// // 	    name_vector, result, args.size(),
// // 	    [&](string_t name) {
// // 			return StringVector::AddString(result, "Trie "+name.GetString()+" 🐥");
// //         });
// // }

// // inline void TrieOpenSSLVersionScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
// //     auto &name_vector = args.data[0];
// //     UnaryExecutor::Execute<string_t, string_t>(
// // 	    name_vector, result, args.size(),
// // 	    [&](string_t name) {
// // 			return StringVector::AddString(result, "Trie " + name.GetString() +
// //                                                      ", my linked OpenSSL version is " +
// //                                                      OPENSSL_VERSION_TEXT );
// //         });
// // }

// // static void LoadInternal(DatabaseInstance &instance) {
// //     // Register a scalar function
// //     auto trie_scalar_function = ScalarFunction("trie", {LogicalType::VARCHAR}, LogicalType::VARCHAR, TrieScalarFun);
// //     ExtensionUtil::RegisterFunction(instance, trie_scalar_function);

// //     // Register another scalar function
// //     auto trie_openssl_version_scalar_function = ScalarFunction("trie_openssl_version", {LogicalType::VARCHAR},
// //                                                 LogicalType::VARCHAR, TrieOpenSSLVersionScalarFun);
// //     ExtensionUtil::RegisterFunction(instance, trie_openssl_version_scalar_function);
// // }

// // void TrieExtension::Load(DuckDB &db) {
// // 	LoadInternal(*db.instance);
// // }
// // std::string TrieExtension::Name() {
// // 	return "trie";
// // }

// // std::string TrieExtension::Version() const {
// // #ifdef EXT_VERSION_TRIE
// // 	return EXT_VERSION_TRIE;
// // #else
// // 	return "";
// // #endif
// // }

// // } // namespace duckdb

// // extern "C" {

// // DUCKDB_EXTENSION_API void trie_init(duckdb::DatabaseInstance &db) {
// //     duckdb::DuckDB db_wrapper(db);
// //     db_wrapper.LoadExtension<duckdb::TrieExtension>();
// // }

// // DUCKDB_EXTENSION_API const char *trie_version() {
// // 	return duckdb::DuckDB::LibraryVersion();
// // }
// // }

// // #ifndef DUCKDB_EXTENSION_MAIN
// // #error DUCKDB_EXTENSION_MAIN not defined
// // #endif
// // #include "duckdb.hpp"
// // #include "trie.hpp"

// // extern "C" {

// // DUCKDB_EXTENSION_API void duckdb_extension_init(duckdb::DatabaseInstance &db) {
// //     Connection conn(db);

// //     conn.CreateScalarFunction("trie_add", {LogicalType::VARCHAR}, LogicalType::VARCHAR, TrieAddFun);
// //     conn.CreateScalarFunction("trie_search", {LogicalType::VARCHAR}, LogicalType::BOOLEAN, TrieSearchFun);
// //     conn.CreateScalarFunction("trie_delete", {LogicalType::VARCHAR}, LogicalType::VARCHAR, TrieDeleteFun);

// //     conn.CreateTableFunction(TableFunction("trie_prefix_search", {LogicalType::VARCHAR}, TriePrefixFunction::Function, TriePrefixFunction::Bind));
// //     conn.CreateTableFunction(TableFunction("trie_wildcard_search", {LogicalType::VARCHAR}, TrieWildcardFunction::Function, TrieWildcardFunction::Bind));
// // }

// // DUCKDB_EXTENSION_API const char *duckdb_library_version() {
// //     return DUCKDB_VERSION;
// // }

// // }


#define DUCKDB_EXTENSION_MAIN

#include "trie_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/function/table_function.hpp"
#include "duckdb/main/extension_util.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>
#include <openssl/opensslv.h>


//  Include your trie function definitions
#include "trie_functions.hpp"  // <-- this header should declare TrieAddFun, TrieSearchFun, etc.

namespace duckdb {

// Built-in scalar function for demo
inline void TrieScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &name_vector = args.data[0];
	UnaryExecutor::Execute<string_t, string_t>(
		name_vector, result, args.size(),
		[&](string_t name) {
			return StringVector::AddString(result, "Trie " + name.GetString() + " 🐥");
		});
}

//  Another scalar fun with OpenSSL version
inline void TrieOpenSSLVersionScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &name_vector = args.data[0];
	UnaryExecutor::Execute<string_t, string_t>(
		name_vector, result, args.size(),
		[&](string_t name) {
			return StringVector::AddString(result, "Trie " + name.GetString() +
			                                     ", my linked OpenSSL version is " + OPENSSL_VERSION_TEXT);
		});
}

static void LoadInternal(DatabaseInstance &instance) {
	// 🧪 Register built-in toy examples
	auto trie_scalar_function = ScalarFunction("trie", {LogicalType::VARCHAR}, LogicalType::VARCHAR, TrieScalarFun);
	ExtensionUtil::RegisterFunction(instance, trie_scalar_function);

	auto trie_openssl_version_function =
	    ScalarFunction("trie_openssl_version", {LogicalType::VARCHAR}, LogicalType::VARCHAR, TrieOpenSSLVersionScalarFun);
	ExtensionUtil::RegisterFunction(instance, trie_openssl_version_function);

	// ✅ Register your actual Trie functions here:
	ExtensionUtil::RegisterFunction(instance, ScalarFunction("trie_add", {LogicalType::VARCHAR}, LogicalType::VARCHAR, TrieAddFun));
	ExtensionUtil::RegisterFunction(instance, ScalarFunction("trie_search", {LogicalType::VARCHAR}, LogicalType::BOOLEAN, TrieSearchFun));
	ExtensionUtil::RegisterFunction(instance, ScalarFunction("trie_delete", {LogicalType::VARCHAR}, LogicalType::VARCHAR, TrieDeleteFun));

	ExtensionUtil::RegisterFunction(instance, TableFunction("trie_prefix_search", {LogicalType::VARCHAR}, TriePrefixFunction::Function, TriePrefixFunction::Bind));
	ExtensionUtil::RegisterFunction(instance, TableFunction("trie_wildcard_search", {LogicalType::VARCHAR}, TrieWildcardFunction::Function, TrieWildcardFunction::Bind));
}

void TrieExtension::Load(DuckDB &db) {
	LoadInternal(*db.instance);
}

std::string TrieExtension::Name() {
	return "trie";
}

std::string TrieExtension::Version() const {
#ifdef EXT_VERSION_TRIE
	return EXT_VERSION_TRIE;
#else
	return "";
#endif
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void trie_init(duckdb::DatabaseInstance &db) {
	duckdb::DuckDB db_wrapper(db);
	db_wrapper.LoadExtension<duckdb::TrieExtension>();
}

DUCKDB_EXTENSION_API const char *trie_version() {
	return duckdb::DuckDB::LibraryVersion();
}

}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif