#!/usr/bin/env python3
import duckdb
import time
import numpy as np
import random
import os

def time_reads(con, query, params_list):
    # Warm-up
    for p in params_list[:100]:
        con.execute(query, p).fetchone()
    # Timed runs
    t0 = time.perf_counter()
    for p in params_list:
        con.execute(query, p).fetchone()
    return time.perf_counter() - t0

def time_writes(con, query, params_list):
    # Timed runs (no warm-up)
    t0 = time.perf_counter()
    for p in params_list:
        con.execute(query, p)
    return time.perf_counter() - t0

def main():
    # keep connection/loading unchanged
    con = duckdb.connect(":memory:", config={"allow_unsigned_extensions": "true"})
    ext = os.path.join(
        os.path.dirname(__file__),
        "build", "release", "extension", "trie", "trie.duckdb_extension"
    )
    con.execute(f"LOAD '{ext}';")

    # Load word list
    with open("words_alpha.txt") as f:
        words = [w.strip() for w in f if w.strip()]
    random.shuffle(words)

    # Fixed parameters
    VOCAB_SIZE  = 1000
    PREFIX_LEN  = 3
    DATASET_SIZES = [1000, 10000, 20000, 50000]

    # Print header
    print("Size  | Dataset | TableIns(s) | TrieIns(s) | TableSearch(s) | TrieSearch(s) | TablePref(s) | TriePref(s)")
    print("------|---------|-------------|------------|----------------|---------------|--------------|-----------")

    for size in DATASET_SIZES:
        # Prepare Uniform keys
        uniform_keys = [(w,) for w in random.sample(words, size)]
        # Prepare Skewed keys via Zipf over top VOCAB_SIZE
        vocab = words[:VOCAB_SIZE]
        ranks = np.arange(1, VOCAB_SIZE + 1)
        weights = 1.0 / np.power(ranks, 1.2)
        weights /= weights.sum()
        choices = np.random.choice(VOCAB_SIZE, size=size, p=weights)
        skewed_keys = [(vocab[i],) for i in choices]

        for label, keys in [("Uniform", uniform_keys), ("Skewed", skewed_keys)]:
            # Reset baseline table
            con.execute("DROP TABLE IF EXISTS kv;")
            con.execute("CREATE TABLE kv(key VARCHAR);")

            # Insert benchmarks
            t_table_ins = time_writes(con, "INSERT INTO kv VALUES (?);", keys)
            t_trie_ins  = time_writes(con, "SELECT trie_add(?);",       keys)

            # Exact‐match search benchmarks
            t_table_srch = time_reads(con, "SELECT COUNT(*) FROM kv WHERE key = ?;", keys)
            t_trie_srch  = time_reads(con, "SELECT trie_search(?);",                keys)

            # Prefix‐search benchmarks
            prefixes = [(k[0][:PREFIX_LEN],) for k in keys]
            t_table_pref = time_reads(
                con,
                "SELECT COUNT(*) FROM kv WHERE key LIKE ?;",
                [(p[0] + '%',) for p in prefixes]
            )
            t_trie_pref = time_reads(
                con,
                "SELECT COUNT(*) FROM trie_prefix_search(?);",
                prefixes
            )

            # Print a row
            print(
                f"{size:5d} | {label:7}| "
                f"{t_table_ins:11.4f}| {t_trie_ins:10.4f}| "
                f"{t_table_srch:14.4f}| {t_trie_srch:13.4f}| "
                f"{t_table_pref:13.4f}| {t_trie_pref:11.4f}"
            )

if __name__ == "__main__":
    main()