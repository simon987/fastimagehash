#ifndef FASTIMAGEHASH_FASTIMAGEHASH_H
#define FASTIMAGEHASH_FASTIMAGEHASH_H

#include <cstdio>

void ahash(void *buf, size_t buf_len, int hash_size);

void dhash(void *buf, size_t buf_len, int hash_size);

void whash(void* buf, size_t buf_len, int hash_size, int img_scale);

void phash(void* buf, size_t buf_len, int hash_size, int highfreq_factor);

#endif
