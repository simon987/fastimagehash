#!/usr/bin/env python
import timeit
import sys

IMAGE = sys.argv[1]
COUNT = 20
SIZE = 8

def print_result(method, time):
    print("%s_%s,%d" % (IMAGE, method, time / COUNT * 1000000000))

print_result("phash", timeit.timeit(
    setup="from imagehash import phash \n"
          "from PIL import Image",
    stmt="phash(Image.open('%s'), hash_size=%d)" % (IMAGE, SIZE),
    number=COUNT
))

print_result("whash", timeit.timeit(
    setup="from imagehash import whash \n"
          "from PIL import Image",
    stmt="whash(Image.open('%s'), hash_size=%d, remove_max_haar_ll=False)" % (IMAGE, SIZE),
    number=COUNT
))

print_result("dhash", timeit.timeit(
    setup="from imagehash import dhash \n"
          "from PIL import Image",
    stmt="dhash(Image.open('%s'), hash_size=%d)" % (IMAGE, SIZE),
    number=COUNT
))

print_result("ahash", timeit.timeit(
    setup="from imagehash import average_hash \n"
          "from PIL import Image",
    stmt="average_hash(Image.open('%s'), hash_size=%d)" % (IMAGE, SIZE),
    number=COUNT
))
