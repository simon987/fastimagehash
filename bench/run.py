from subprocess import check_output, DEVNULL
import csv

files = (
    "100px",
    "200px",
    "300px",
    "500px",
    "1000px",
    "2000px",
    "3000px",
    "4000px",
    "6000px",
    "8000px",
)

for f in files:
    out = check_output(["python3", "benchmark.py", f])
    print(out.decode(), end="")

print()

for f in files:
    out = check_output(["./bm", f, "--benchmark_format=csv"], stderr=DEVNULL)

    for line in out.decode().splitlines(keepends=False):
        if line.startswith("\"BM_"):
            m, _, t, *_ = line.split(",")
            if "phash" in m:
                method = "phash"
            if "dhash" in m:
                method = "dhash"
            if "ahash" in m:
                method = "ahash"
            if "whash" in m:
                method = "whash"
            print("%s_%s,%s" % (f, method, t))
