#ifndef FASTIMAGEHASH_FASTIMAGEHASH_H
#define FASTIMAGEHASH_FASTIMAGEHASH_H

#define FASTIMAGEHASH_VERSION "4.0"


#include <stdio.h>

typedef unsigned char uchar;

#ifdef __cplusplus
extern "C" {
#endif

void hash_to_hex_string_reversed(const uchar *h, char *out, int hash_size);

void hash_to_hex_string(const uchar *h, char *out, int hash_size);

int mhash_mem(void *buf, size_t buf_len,uchar *out, int hash_size);

int mhash_file(const char *filepath, uchar *out, int hash_size);

int ahash_mem(void *buf, size_t buf_len, uchar *out, int hash_size);

int ahash_file(const char *filepath, uchar *out, int hash_size);

int dhash_file(const char *filepath, uchar *out, int hash_size);

int dhash_mem(void *buf, size_t buf_len, uchar *out, int hash_size);

int whash_file(const char *filepath, uchar *out, int hash_size, int img_scale, int remove_max_ll, const char* wname);

int whash_mem(void *buf, size_t buf_len, uchar *out, int hash_size, int img_scale, int remove_max_ll, const char*wname);

int phash_file(const char *buf, uchar *out, int hash_size, int highfreq_factor);

int phash_mem(void *buf, size_t buf_len, uchar *out, int hash_size, int highfreq_factor);

#ifdef __cplusplus
};
#endif


#endif
