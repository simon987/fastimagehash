#ifndef FASTIMAGEHASH_FASTIMAGEHASH_H
#define FASTIMAGEHASH_FASTIMAGEHASH_H

#define FASTIMAGEHASH_VERSION "1.0"


#include <stdio.h>

typedef unsigned char uchar;

#ifdef __cplusplus
extern "C" {
#endif

void hash_to_hex_string_reversed(const uchar *h, char *out, int hash_size);

void hash_to_hex_string(const uchar *h, char *out, int hash_size);

/**
 *
 * @param buf
 * @param buf_len
 * @param hash_size
 */
int ahash_mem(void *buf, uchar *out, size_t buf_len, int hash_size);

int ahash_file(const char *filepath, uchar *out, int hash_size);

int dhash_file(const char *filepath, uchar *out, int hash_size);

int dhash_mem(void *buf, uchar *out, size_t buf_len, int hash_size);

int whash_file(const char *filepath, uchar *out, int hash_size, int img_scale);

int whash_mem(void *buf, uchar *out, size_t buf_len, int hash_size, int img_scale);

int phash_file(const char *buf, uchar *out, int hash_size, int highfreq_factor);

int phash_mem(void *buf, uchar *out, size_t buf_len, int hash_size, int highfreq_factor);

#ifdef __cplusplus
};
#endif


#endif
