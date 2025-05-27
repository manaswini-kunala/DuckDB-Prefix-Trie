#!/usr/bin/env python3
import duckdb
import time
import random
import os

def batch_prefix_benchmark(con, prefixes):
    # Build one big UNION ALL query for the table+INDEX approach
    sql_table = " UNION ALL ".join(
        f"SELECT '{p}' AS prefix, COUNT(*) AS cnt FROM data WHERE word LIKE '{p}%'"
        for p in prefixes
    )
    # Build one big UNION ALL query for the trie extension approach
    sql_trie = " UNION ALL ".join(
        f"SELECT '{p}' AS prefix, COUNT(*) AS cnt FROM trie_prefix_search('{p}') AS t(word)"
        for p in prefixes
    )

    # Time the table+INDEX version
    t0 = time.perf_counter()
    con.execute(sql_table).fetchall()
    t_table = time.perf_counter() - t0

    # Time the trie extension version
    t0 = time.perf_counter()
    con.execute(sql_trie).fetchall()
    t_trie = time.perf_counter() - t0

    return t_table, t_trie

def main():
    # keep connection/loading unchanged
    con = duckdb.connect(":memory:", config={"allow_unsigned_extensions": "true"})
    ext = os.path.join(
        os.path.dirname(__file__),
        "build", "release", "extension", "trie", "trie.duckdb_extension"
    )
    con.execute(f"LOAD '{ext}';")

    # 👉 Raise the parser’s max expression depth so large UNION ALLs parse
    con.execute("SET max_expression_depth TO 5000;")

    # parameters
    N_ROWS       = 100_000    # number of rows in data
    PREFIX_COUNT =   1_000    # how many prefixes to test
    PREFIX_LEN   =       3    # length of each prefix

    # 1) Populate data
    print(f"Inserting {N_ROWS} rows into `data`…")
    con.execute("CREATE TABLE data(word VARCHAR);")
    con.execute(f"""
        INSERT INTO data
        SELECT SUBSTR(md5(random()::TEXT),1,10)
        FROM range({N_ROWS});
    """)
    print("Done inserting.\n")

    # 2) Create an index on data(word)
    print("Creating index on data(word)…")
    con.execute("CREATE INDEX idx_data_word ON data(word);")
    print("Index created.\n")

    # 3) Sample random prefixes
    print(f"Sampling {PREFIX_COUNT} random prefixes of length {PREFIX_LEN}…")
    rows = con.execute(f"""
        SELECT DISTINCT SUBSTR(word,1,{PREFIX_LEN}) AS pref
        FROM data
        ORDER BY random()
        LIMIT {PREFIX_COUNT}
    """).fetchall()
    prefixes = [r[0] for r in rows]
    print("Sampled prefixes.\n")

    # 4) Run the batch benchmark
    print("Running batch prefix-scan benchmark…")
    t_table, t_trie = batch_prefix_benchmark(con, prefixes)

    # 5) Report
    print("\nBatch prefix-scan timings:")
    print(f"  DuckDB LIKE + INDEX : {t_table:.3f} s")
    print(f"  Trie Extension      : {t_trie:.3f} s")

if __name__ == "__main__":
    main()