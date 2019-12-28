#include "fastimagehash.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <wavelib.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fftw3.h>
#include <sys/stat.h>

static void init() __attribute__((constructor));
void init() {
    fftw_make_planner_thread_safe();
}

using namespace cv;

#define FASTIMAGEHASH_OK 0
#define FASTIMAGEHASH_READ_ERR -2
#define FASTIMAGEHASH_DECODE_ERR -3

__always_inline
double median(double *arr, size_t len) {
    std::sort(arr, arr + len);

    if (len % 2 == 0) {
        return (arr[(len / 2) - 1] + arr[len / 2]) / 2;
    } else {
        return arr[(len + 1 / 2)];
    }
}

__always_inline
double median(uchar *arr, size_t len) {
    std::sort(arr, arr + len);

    if (len % 2 == 0) {
        return (double)(arr[(len / 2) - 1] + arr[len / 2]) / 2;
    } else {
        return arr[(len + 1 / 2)];
    }
}

void hash_to_hex_string(const uchar *h, char *out, int hash_size) {
    int hash_len = hash_size * hash_size / 4;

    for (unsigned int i = 0; i < hash_len; i += 2) {
        sprintf(out + i, "%02x", h[i / 2]);
    }
    out[hash_len + 1] = '\0';
}

void hash_to_hex_string_reversed(const uchar *h, char *out, int hash_size) {
    int hash_len = hash_size * hash_size / 4;

    for (unsigned int i = 0; i < hash_len; i += 2) {
        uchar c = (h[i / 2] & 0x80) >> 7 |
                  (h[i / 2] & 0x40) >> 5 |
                  (h[i / 2] & 0x20) >> 3 |
                  (h[i / 2] & 0x10) >> 1 |
                  (h[i / 2] & 0x08) << 1 |
                  (h[i / 2] & 0x04) << 3 |
                  (h[i / 2] & 0x02) << 5 |
                  (h[i / 2] & 0x01) << 7;

        sprintf(out + i, "%02x", c);
    }
    out[hash_len + 1] = '\0';
}

__always_inline
void set_bit_at(uchar *buf, unsigned int offset, bool val) {
    unsigned int byte_offset = offset / 8;
    unsigned int bit_offset = offset - byte_offset * 8;

    if (val) {
        buf[byte_offset] |= (1 << bit_offset);
    } else {
        buf[byte_offset] &= ~(1 << bit_offset);
    }
}

void *load_file_in_mem(const char *filepath, size_t *size) {
    struct stat info{};
    if (stat(filepath, &info) != 0) {
        return nullptr;
    }

    FILE *file = fopen(filepath, "rb");

    if (file == nullptr) {
        return nullptr;
    }

    void *buf = malloc(info.st_size);
    fread(buf, sizeof(char), info.st_size, file);

    *size = info.st_size;
    return buf;
}

int ahash_file(const char *filepath, uchar *out, int hash_size) {
    size_t size;
    void *buf = load_file_in_mem(filepath, &size);

    if (buf == nullptr) {
        return FASTIMAGEHASH_READ_ERR;
    }

    int ret = ahash_mem(buf, out, size, hash_size);
    free(buf);
    return ret;
}

int ahash_mem(void *buf, uchar *out, size_t buf_len, int hash_size) {
    Mat im;
    try {
        im = imdecode(Mat(1, buf_len, CV_8UC1, buf), IMREAD_GRAYSCALE);
        resize(im, im, Size(hash_size, hash_size), 0, 0, INTER_AREA);
    } catch (Exception &e) {
        return FASTIMAGEHASH_DECODE_ERR;
    }

    double avg = mean(im).val[0];

    uchar *pixel = im.ptr(0);
    int endPixel = im.cols * im.rows;
    for (int i = 0; i < endPixel; i++) {
        set_bit_at(out, i, pixel[i] > avg);
    }
    return FASTIMAGEHASH_OK;
}

int mhash_file(const char *filepath, uchar *out, int hash_size) {
    size_t size;
    void *buf = load_file_in_mem(filepath, &size);

    if (buf == nullptr) {
        return FASTIMAGEHASH_READ_ERR;
    }

    int ret = mhash_mem(buf, out, size, hash_size);
    free(buf);
    return ret;
}

int mhash_mem(void *buf, uchar *out, size_t buf_len, int hash_size) {
    Mat im;
    try {
        im = imdecode(Mat(1, buf_len, CV_8UC1, buf), IMREAD_GRAYSCALE);
        resize(im, im, Size(hash_size, hash_size), 0, 0, INTER_AREA);
    } catch (Exception &e) {
        return FASTIMAGEHASH_DECODE_ERR;
    }

    uchar *pixel = im.ptr(0);
    const int endPixel = im.cols * im.rows;

    uchar sorted[im.cols * im.rows];
    memcpy(sorted, pixel, endPixel);
    double med = median(sorted, endPixel);

    for (int i = 0; i < endPixel; i++) {
        set_bit_at(out, i, pixel[i] > med);
    }

    return FASTIMAGEHASH_OK;
}

int dhash_file(const char *filepath, uchar *out, int hash_size) {
    size_t size;
    void *buf = load_file_in_mem(filepath, &size);
    if (buf == nullptr) {
        return FASTIMAGEHASH_READ_ERR;
    }

    int ret = dhash_mem(buf, out, size, hash_size);
    free(buf);
    return ret;
}

int dhash_mem(void *buf, uchar *out, size_t buf_len, int hash_size) {
    Mat im;
    try {
        im = imdecode(Mat(1, buf_len, CV_8UC1, buf), IMREAD_GRAYSCALE);
        resize(im, im, Size(hash_size + 1, hash_size), 0, 0, INTER_AREA);
    } catch (Exception &e) {
        return FASTIMAGEHASH_DECODE_ERR;
    }

    int offset = 0;
    for (int i = 0; i < im.rows; ++i) {
        uchar *pixel = im.ptr(i);

        for (int j = 1; j < im.cols; ++j) {
            set_bit_at(out, offset++, pixel[j] > pixel[j - 1]);
        }
    }
    return FASTIMAGEHASH_OK;
}

int whash_file(const char *filepath, uchar *out, int hash_size, int img_scale, const char* wname) {
    size_t size;
    void *buf = load_file_in_mem(filepath, &size);
    if (buf == nullptr) {
        return FASTIMAGEHASH_READ_ERR;
    }

    int ret = whash_mem(buf, out, size, hash_size, img_scale, wname);
    free(buf);
    return ret;
}

int whash_mem(void *buf, uchar *out, size_t buf_len, const int hash_size, int img_scale, const char *wname) {
    Mat im;
    try {
        im = imdecode(Mat(1, buf_len, CV_8UC1, buf), IMREAD_GRAYSCALE);
    } catch (Exception &e) {
        return FASTIMAGEHASH_DECODE_ERR;
    }

    if (strcmp(wname, "haar") != 0 && strcmp(wname, "db4") != 0) {
        throw std::invalid_argument("wname must be either of 'haar' or 'db4'");
    }

    if ((hash_size & (hash_size - 1)) != 0) {
        throw std::invalid_argument("hash_size must be a power of two");
    }

    if (img_scale != 0) {
        if ((img_scale & (img_scale - 1)) != 0) {
            throw std::invalid_argument("img_scale must be a power of two");
        }
    } else {
        int image_natural_scale = (int) pow(2, (int) log2(MIN(im.rows, im.cols)));
        img_scale = MAX(image_natural_scale, hash_size);
    }

    int ll_max_level = (int) log2(img_scale);
    int level = (int) log2(hash_size);

    if (ll_max_level < level) {
        throw std::invalid_argument("hash_size in a wrong range");
    }

    int dwt_level = ll_max_level - level;

    try {
        resize(im, im, Size(img_scale, img_scale), 0, 0, INTER_AREA);
    } catch (Exception &e) {
        return FASTIMAGEHASH_DECODE_ERR;
    }

    auto data = (double *) malloc(img_scale * img_scale * sizeof(double));

    uchar *pixel = im.ptr(0);
    const int endPixel = im.cols * im.rows;
    for (int i = 0; i < endPixel; i++) {
        data[i] = (double) pixel[i] / 255;
    }

    wave_object w = wave_init(wname);
    wt2_object wt = wt2_init(w, "dwt", img_scale, img_scale, dwt_level);

    double *coeffs = dwt2(wt, data);

    double sorted[hash_size * hash_size];
    memcpy(sorted, coeffs, sizeof(double) * hash_size * hash_size);

    double med = median(sorted, hash_size * hash_size);

    for (int i = 0; i < hash_size * hash_size; ++i) {
        set_bit_at(out, i, coeffs[i] > med);
    }

    free(data);
    wt2_free(wt);
    wave_free(w);
    free(coeffs);

    return FASTIMAGEHASH_OK;
}

int phash_file(const char *filepath, uchar *out, const int hash_size, int highfreq_factor) {
    size_t size;
    void *buf = load_file_in_mem(filepath, &size);

    if (buf == nullptr) {
        return FASTIMAGEHASH_READ_ERR;
    }

    int ret = phash_mem(buf, out, size, hash_size, highfreq_factor);
    free(buf);
    return ret;
}

int phash_mem(void *buf, uchar *out, size_t buf_len, const int hash_size, int highfreq_factor) {
    int img_size = hash_size * highfreq_factor;

    Mat im;
    try {
        im = imdecode(Mat(1, buf_len, CV_8UC1, buf), IMREAD_GRAYSCALE);
        resize(im, im, Size(img_size, img_size), 0, 0, INTER_AREA);
    } catch (Exception &e) {
        return FASTIMAGEHASH_DECODE_ERR;
    }

    double pixels[img_size * img_size];

    uchar *pixel = im.ptr(0);
    int endPixel = im.cols * im.rows;
    for (int i = 0; i < endPixel; i++) {
        pixels[i] = (double) pixel[i] / 255;
    }

    double dct_out[img_size * img_size];
    fftw_plan plan = fftw_plan_r2r_2d(
            img_size, img_size,
            pixels, dct_out,
            FFTW_REDFT10, FFTW_REDFT10, // DCT-II
            FFTW_ESTIMATE
    );
    fftw_execute(plan);
    fftw_destroy_plan(plan);

    double dct_lowfreq[hash_size * hash_size];
    double sorted[hash_size * hash_size];

    int ptr_low = 0;
    int ptr = 0;
    for (int i = 0; i < hash_size; ++i) {
        for (int j = 0; j < hash_size; ++j) {
            dct_lowfreq[ptr_low] = dct_out[ptr];
            sorted[ptr_low] = dct_out[ptr];
            ptr_low += 1;
            ptr += 1;
        }
        ptr += (img_size - hash_size);
    }

    double med = median(sorted, hash_size * hash_size);

    for (int i = 0; i < hash_size * hash_size; ++i) {
        set_bit_at(out, i, dct_lowfreq[i] > med);
    }
    return FASTIMAGEHASH_OK;
}

multi_hash_t *multi_hash_create(int hash_size) {
    auto multi_hash = (multi_hash_t *) malloc(sizeof(multi_hash_t));
    auto data = (uchar *) malloc((hash_size + 1) * 5);

    multi_hash->ahash = data;
    multi_hash->phash = data + (hash_size + 1);
    multi_hash->dhash = data + (hash_size + 1) * 2;
    multi_hash->whash = data + (hash_size + 1) * 3;
    multi_hash->mhash = data + (hash_size + 1) * 4;

    return multi_hash;
}

void multi_hash_destroy(multi_hash_t *h) {
    free(h->ahash);
    free(h);
}

int multi_hash_file(const char *filepath, multi_hash_t *out, int hash_size,
                    int ph_highfreq_factor, int wh_img_scale, const char* wname) {
    size_t size;
    void *buf = load_file_in_mem(filepath, &size);

    if (buf == nullptr) {
        return FASTIMAGEHASH_READ_ERR;
    }

    int ret = multi_hash_mem(buf, out, size, hash_size, ph_highfreq_factor, wh_img_scale, wname);
    free(buf);
    return ret;
}

int multi_hash_mem(void *buf, multi_hash_t *out, size_t buf_len,
                   int hash_size, int ph_highfreq_factor, int wh_img_scale,
                   const char*wname) {
    Mat im;
    try {
        im = imdecode(Mat(1, buf_len, CV_8UC1, buf), IMREAD_GRAYSCALE);
    } catch (Exception &e) {
        return FASTIMAGEHASH_DECODE_ERR;
    }

    Mat ahash_im; // Also used for mhash!
    Mat dhash_im;
    Mat phash_im;
    Mat whash_im;

    int ph_img_scale = hash_size * ph_highfreq_factor;

    if ((hash_size & (hash_size - 1)) != 0) {
        throw std::invalid_argument("hash_size must be a power of two");
    }

    if (wh_img_scale != 0) {
        if ((wh_img_scale & (wh_img_scale - 1)) != 0) {
            throw std::invalid_argument("wh_img_scale must be a power of two");
        }
    } else {
        int image_natural_scale = (int) pow(2, (int) log2(MIN(im.rows, im.cols)));
        wh_img_scale = MAX(image_natural_scale, hash_size);
    }

    int ll_max_level = (int) log2(wh_img_scale);
    int level = (int) log2(hash_size);

    if (ll_max_level < level) {
        throw std::invalid_argument("hash_size in a wrong range");
    }

    int dwt_level = ll_max_level - level;

    try {
        im = imdecode(Mat(1, buf_len, CV_8UC1, buf), IMREAD_GRAYSCALE);

        resize(im, ahash_im, Size(hash_size, hash_size), 0, 0, INTER_AREA);
        resize(im, dhash_im, Size(hash_size + 1, hash_size), 0, 0, INTER_AREA);
        resize(im, whash_im, Size(wh_img_scale, wh_img_scale), 0, 0, INTER_AREA);
        resize(im, phash_im, Size(ph_img_scale, ph_img_scale), 0, 0, INTER_AREA);
    } catch (Exception &e) {
        return FASTIMAGEHASH_DECODE_ERR;
    }

    auto pixels = new double[MAX(ph_img_scale, wh_img_scale) * MAX(ph_img_scale, wh_img_scale)];

    // ahash
    double avg = mean(ahash_im).val[0];

    uchar *pixel = ahash_im.ptr(0);
    int endPixel = ahash_im.cols * ahash_im.rows;

    // mhash
    uchar mhash_sorted [ahash_im.cols * ahash_im.rows];
    mempcpy(mhash_sorted, pixel, endPixel);
    double m_median = median(mhash_sorted, endPixel);

    for (int i = 0; i < endPixel; i++) {
        set_bit_at(out->ahash, i, pixel[i] > avg);
        set_bit_at(out->mhash, i, pixel[i] > m_median);
    }

    //dhash
    int offset = 0;
    for (int i = 0; i < dhash_im.rows; ++i) {
        pixel = dhash_im.ptr(i);

        for (int j = 1; j < dhash_im.cols; ++j) {
            set_bit_at(out->dhash, offset++, pixel[j] > pixel[j - 1]);
        }
    }

    //phash
    pixel = phash_im.ptr(0);
    endPixel = phash_im.cols * phash_im.rows;
    for (int i = 0; i < endPixel; i++) {
        pixels[i] = (double) pixel[i] / 255;
    }

    double dct_out[ph_img_scale * ph_img_scale];
    fftw_plan plan = fftw_plan_r2r_2d(
            ph_img_scale, ph_img_scale,
            pixels, dct_out,
            FFTW_REDFT10, FFTW_REDFT10, // DCT-II
            FFTW_ESTIMATE
    );
    fftw_execute(plan);
    fftw_destroy_plan(plan);

    double dct_lowfreq[hash_size * hash_size];
    double sorted[hash_size * hash_size];

    int ptr_low = 0;
    int ptr = 0;
    for (int i = 0; i < hash_size; ++i) {
        for (int j = 0; j < hash_size; ++j) {
            dct_lowfreq[ptr_low] = dct_out[ptr];
            sorted[ptr_low] = dct_out[ptr];
            ptr_low += 1;
            ptr += 1;
        }
        ptr += (ph_img_scale - hash_size);
    }

    double med = median(sorted, hash_size * hash_size);

    for (int i = 0; i < hash_size * hash_size; ++i) {
        set_bit_at(out->phash, i, dct_lowfreq[i] > med);
    }

    //whash
    pixel = whash_im.ptr(0);
    endPixel = whash_im.cols * whash_im.rows;
    for (int i = 0; i < endPixel; i++) {
        pixels[i] = (double) pixel[i] / 255;
    }

    wave_object w = wave_init(wname);
    wt2_object wt = wt2_init(w, "dwt", wh_img_scale, wh_img_scale, dwt_level);

    double *coeffs = dwt2(wt, pixels);

    memcpy(sorted, coeffs, sizeof(double) * (hash_size * hash_size));

    med = median(sorted, hash_size * hash_size);

    for (int i = 0; i < hash_size * hash_size; ++i) {
        set_bit_at(out->whash, i, coeffs[i] > med);
    }

    wt2_free(wt);
    wave_free(w);
    free(coeffs);
    delete[] pixels;
    return FASTIMAGEHASH_OK;
}
