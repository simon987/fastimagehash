#ifndef FASTIMAGEHASH_FASTIMAGEHASH_H
#define FASTIMAGEHASH_FASTIMAGEHASH_H

#define FASTIMAGEHASH_VERSION "3.0"


#include <stdio.h>

typedef unsigned char uchar;

typedef struct multi_hash {
    uchar *ahash;
    uchar *phash;
    uchar *dhash;
    uchar *whash;
    uchar *mhash;
} multi_hash_t;

#ifdef __cplusplus
extern "C" {
#endif

multi_hash_t *multi_hash_create(int hash_size);

void multi_hash_destroy(multi_hash_t *h);

int multi_hash_file(const char *filepath, multi_hash_t *out, int hash_size, int ph_highfreq_factor, int wh_img_scale, const char*wname);

void hash_to_hex_string_reversed(const uchar *h, char *out, int hash_size);

void hash_to_hex_string(const uchar *h, char *out, int hash_size);

int multi_hash_mem(void *buf, size_t buf_len, multi_hash_t *out, int hash_size, int ph_highfreq_factor, int wh_img_scale, const char* wname);

int mhash_mem(void *buf, size_t buf_len,uchar *out, int hash_size);

int mhash_file(const char *filepath, uchar *out, int hash_size);

int ahash_mem(void *buf, size_t buf_len, uchar *out, int hash_size);

int ahash_file(const char *filepath, uchar *out, int hash_size);

int dhash_file(const char *filepath, uchar *out, int hash_size);

int dhash_mem(void *buf, size_t buf_len, uchar *out, int hash_size);

int whash_file(const char *filepath, uchar *out, int hash_size, int img_scale, const char* wname);

int whash_mem(void *buf, size_t buf_len, uchar *out, int hash_size, int img_scale, const char*wname);

int phash_file(const char *buf, uchar *out, int hash_size, int highfreq_factor);

int phash_mem(void *buf, size_t buf_len, uchar *out, int hash_size, int highfreq_factor);

#ifdef __cplusplus
};
#endif


#endif
