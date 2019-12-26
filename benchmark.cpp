#include <benchmark/benchmark.h>
#include "fastimagehash.h"

#include <fcntl.h>

char *filepath;
uchar tmp[512];

void *load_test_file(size_t *buf_len) {
    FILE *file = fopen(filepath, "rb");

    fseek(file, 0, SEEK_END);
    *buf_len = ftell(file);
    fclose(file);

    void *buf = malloc(*buf_len);
    file = fopen(filepath, "rb");
    fread(buf, *buf_len, 1, file);
    return buf;
}

static void BM_phash(benchmark::State &state) {

    size_t size;
    void *buf = load_test_file(&size);

    for (auto _ : state) {
        phash_mem(buf, tmp, size, state.range(), 4);
    }

    free(buf);
}

static void BM_whash(benchmark::State &state) {

    size_t size;
    void *buf = load_test_file(&size);

    for (auto _ : state) {
        whash_mem(buf, tmp, size, state.range(), 0);
    }

    free(buf);
}

static void BM_dhash(benchmark::State &state) {

    size_t size;
    void *buf = load_test_file(&size);

    for (auto _ : state) {
        dhash_mem(buf, tmp, size, state.range());
    }

    free(buf);
}

static void BM_ahash(benchmark::State &state) {

    size_t size;
    void *buf = load_test_file(&size);

    for (auto _ : state) {
        ahash_mem(buf, tmp, size, state.range());
    }

    free(buf);
}

BENCHMARK(BM_phash)->ArgName("size")->Arg(8);
BENCHMARK(BM_whash)->ArgName("size")->Arg(8);
BENCHMARK(BM_dhash)->ArgName("size")->Arg(8);
BENCHMARK(BM_ahash)->ArgName("size")->Arg(8);


int main(int argc, char **argv) {
    filepath = argv[1];
    argv[1] = argv[0];

    argc -= 1;

    ::benchmark::Initialize(&argc, argv + 1);
    ::benchmark::RunSpecifiedBenchmarks();
}
