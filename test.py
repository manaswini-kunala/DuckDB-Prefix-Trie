# #!/usr/bin/env python3
# import duckdb
# import time
# import os
# import random

# # 0) Allow unsigned extensions globally
# os.environ["DUCKDB_ALLOW_UNSIGNED_EXTENSIONS"] = "1"

# def time_query(con, sql, runs=5):
#     # warm-up
#     con.execute(sql).fetchall()
#     times = []
#     for _ in range(runs):
#         t0 = time.perf_counter()
#         con.execute(sql).fetchall()
#         times.append(time.perf_counter() - t0)
#     return sum(times) / len(times)

# def main():
#     con = duckdb.connect(database=":memory:", config={"allow_unsigned_extensions": "true"})
#     # tell DuckDB to allow unsigned extensions this session
#     # con.execute("PRAGMA allow_unsigned_extensions = true;")

#     # build the absolute path to your extension
#     script_dir = os.path.dirname(os.path.abspath(__file__))
#     ext_path = os.path.join(
#         script_dir,
#         "build", "release", "extension", "trie", "trie.duckdb_extension"
#     )
#     # load it via SQL LOAD (respects the PRAGMA)
#     con.execute(f"LOAD '{ext_path}';")  

#     # now your trie functions are registered under SQL
#     con.execute("LOAD trie;")

#     # prepare test data
#     con.execute("""
#       CREATE OR REPLACE TABLE words AS
#       SELECT SUBSTR(md5(random()::TEXT),1,10) AS word
#       FROM range(500000);
#     """)
#     con.execute("CREATE INDEX IF NOT EXISTS idx_word ON words(word);")

#     # define and time the two approaches
#     prefix = "ab"
#     q_like = f"SELECT COUNT(*) FROM words WHERE word LIKE '{prefix}%';"
#     q_trie = f"SELECT COUNT(*) FROM trie_prefix_search('{prefix}');"

#     avg_like = time_query(con, q_like, runs=10)
#     avg_trie = time_query(con, q_trie, runs=10)

#     print(f"Average with LIKE : {avg_like*1000:.2f} ms")
#     print(f"Average with TRIE : {avg_trie*1000:.2f} ms")

# if __name__ == "__main__":
#     main()



import duckdb, time, random, os

# 0) Allow unsigned extensions
# os.environ["DUCKDB_ALLOW_UNSIGNED_EXTENSIONS"] = "1"

def time_loop(fn, count):
    # single timer around N calls
    t0 = time.perf_counter()
    fn()
    return time.perf_counter() - t0

# def main():
#     # Connect and load extension
#     con = duckdb.connect(":memory:", config={"allow_unsigned_extensions": "true"})
#     # con.execute("PRAGMA allow_unsigned_extensions=TRUE;")
#     ext = os.path.join(
#         os.path.dirname(__file__),
#         "build","release","extension","trie","trie.duckdb_extension"
#     )
#     con.execute(f"LOAD '{ext}';")

#     # Load word list
#     with open("words_alpha.txt") as f:
#         words = [w.strip() for w in f if w.strip()]
#     random.shuffle(words)

#     print("  N | TABLE_INS(s) | TRIE_INS(s) | TABLE_PREFIX_SEARCH(s) | TRIE_PREFIX_SEARCH(s)")
#     print("----|--------------|-------------|------------------------|----------------------")

#     for N in [10, 100, 1000, 10000]:
#         # sample 2N words -> first N=keys
#         sample = words[:2*N]
#         keys = sample[:N]

#         # 1) Baseline: simple table of keys
#         con.execute("DROP TABLE IF EXISTS kv;")
#         con.execute("CREATE TABLE kv(key VARCHAR);")

#         # TIME table inserts
#         def table_inserts():
#             for key in keys:
#                 con.execute("INSERT INTO kv VALUES (?);", (key,))
#         t_table_ins = time_loop(table_inserts, N)

#         # TIME trie inserts
#         def trie_inserts():
#             for key in keys:
#                 con.execute("SELECT trie_add(?)", (key,))
#         t_trie_ins = time_loop(trie_inserts, N)

#         # TIME table lookups
#         def table_searches():
#             for key in keys:
#                 con.execute("SELECT COUNT(*) FROM kv WHERE key = ?;", (key,)).fetchone()
#         t_table_srch = time_loop(table_searches, N)

#         # TIME trie lookups
#         def trie_searches():
#             for key in keys:
#                 con.execute("SELECT trie_search(?)", (key,)).fetchone()
#         t_trie_srch = time_loop(trie_searches, N)

#         print(f"{N:4d}| {t_table_ins:12.4f}| {t_trie_ins:12.4f}| {t_table_srch:14.4f}| {t_trie_srch:13.4f}")


def main():
    # Connect and load extension
    con = duckdb.connect(":memory:", config={"allow_unsigned_extensions": "true"})
    ext = os.path.join(
        os.path.dirname(__file__),
        "build","release","extension","trie","trie.duckdb_extension"
    )
    con.execute(f"LOAD '{ext}';")

    # Load word list
    with open("words_alpha.txt") as f:
        words = [w.strip() for w in f if w.strip()]
    random.shuffle(words)

    print("  N | TABLE_INS | TRIE_INS | TABLE_SEARCH | TRIE_SEARCH | TABLE_PREF | TRIE_PREF")
    print("----|-----------|----------|--------------|-------------|------------|-----------")

    PREFIX_LEN = 3  # prefix length for prefix‐search

    for N in [10, 100, 1000, 10000]:
        sample   = words[:2*N]
        keys     = sample[:N]
        prefixes = [k[:PREFIX_LEN] for k in keys]

        # reset table
        con.execute("DROP TABLE IF EXISTS kv;")
        con.execute("CREATE TABLE kv(key VARCHAR);")

        # 1) Table inserts
        def table_inserts():
            for key in keys:
                con.execute("INSERT INTO kv VALUES (?);", (key,))
        t_table_ins = time_loop(table_inserts, N)

        # 2) Trie inserts
        def trie_inserts():
            for key in keys:
                con.execute("SELECT trie_add(?)", (key,))
        t_trie_ins = time_loop(trie_inserts, N)

        # 3) Table whole-key searches
        def table_searches():
            for key in keys:
                con.execute("SELECT COUNT(*) FROM kv WHERE key = ?;", (key,)).fetchone()
        t_table_search = time_loop(table_searches, N)

        # 4) Trie whole-key searches
        def trie_searches():
            for key in keys:
                con.execute("SELECT trie_search(?)", (key,)).fetchone()
        t_trie_search = time_loop(trie_searches, N)

        # 5) Table prefix searches
        def table_prefix_searches():
            for p in prefixes:
                con.execute("SELECT COUNT(*) FROM kv WHERE key LIKE ?;", (p + '%',)).fetchone()
        t_table_pref = time_loop(table_prefix_searches, N)

        # 6) Trie prefix searches
        def trie_prefix_searches():
            for p in prefixes:
                con.execute("SELECT COUNT(*) FROM trie_prefix_search(?);", (p,)).fetchone()
        t_trie_pref = time_loop(trie_prefix_searches, N)

        print(
            f"{N:4d}| "
            f"{t_table_ins:9.4f}| "
            f"{t_trie_ins:9.4f}| "
            f"{t_table_search:13.4f}| "
            f"{t_trie_search:12.4f}| "
            f"{t_table_pref:11.4f}| "
            f"{t_trie_pref:10.4f}"
        )

if __name__ == "__main__":
    main()