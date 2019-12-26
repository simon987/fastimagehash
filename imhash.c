#include "fastimagehash.h"
#include <stdio.h>
#include <string.h>

void print_usage() {
    printf("Using fastimagehash library v%s\n", FASTIMAGEHASH_VERSION);
    printf("Usage: imhash [FILE]...\n");
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        print_usage();
        return 0;
    }

    int do_phash = 0, do_ahash = 0, do_whash = 0, do_dhash = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--phash") == 0) {
            do_phash = 1;
        } else if (strcmp(argv[i], "--ahash") == 0) {
            do_ahash = 1;
        } else if (strcmp(argv[i], "--whash") == 0) {
            do_whash = 1;
        } else if (strcmp(argv[i], "--dhash") == 0) {
            do_dhash = 1;
        } else {

            uchar hash[9];
            char hashstr[17];

            if (do_phash) {
                if (phash_file(argv[i], hash, 8, 4) == 0) {
                    hash_to_hex_string_reversed(hash, hashstr, 8);
                    printf("%s\tp:%s\n", argv[i], hashstr);
                }
            }
            if (do_ahash) {
                if (ahash_file(argv[i], hash, 8) == 0) {
                    hash_to_hex_string_reversed(hash, hashstr, 8);
                    printf("%s\ta:%s\n", argv[i], hashstr);
                }
            }
            if (do_dhash) {
                if (dhash_file(argv[i], hash, 8) == 0) {
                    hash_to_hex_string_reversed(hash, hashstr, 8);
                    printf("%s\td:%s\n", argv[i], hashstr);
                }
            }
            if (do_whash) {
                if (whash_file(argv[i], hash, 8, 0) == 0) {
                    hash_to_hex_string_reversed(hash, hashstr, 8);
                    printf("%s\tw:%s\n", argv[i], hashstr);
                }
            }

            multi_hash_t *m = multi_hash_create(8);
            multi_hash_file(argv[i], m, 8, 4, 0);

            hash_to_hex_string_reversed(m->phash, hashstr, 8);
            printf("%s\tmp:%s\n", argv[i], hashstr);
            hash_to_hex_string_reversed(m->ahash, hashstr, 8);
            printf("%s\tma:%s\n", argv[i], hashstr);
            hash_to_hex_string_reversed(m->dhash, hashstr, 8);
            printf("%s\tmd:%s\n", argv[i], hashstr);
            hash_to_hex_string_reversed(m->whash, hashstr, 8);
            printf("%s\tmw:%s\n", argv[i], hashstr);
        }
    }
}