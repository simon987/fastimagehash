#include "fastimagehash.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <wavelib.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fftw3.h>

using namespace cv;

__always_inline
double median(double *arr, size_t len) {
    std::sort(arr, arr + len);

    //todo: odd len
    return (arr[(len / 2) - 1] + arr[len / 2]) / 2;
}

void printBitSet(std::vector<bool> *bs) {

    int len = bs->size();

    for (int i = 3; i <= len; i += 4) {
        std::cout << std::hex <<
                  (((*bs)[i - 3] << 3) | ((*bs)[i - 2] << 2) | ((*bs)[i - 1] << 1) | ((*bs)[i]));

    }
    std::cout << std::endl;
}

void ahash(void *buf, size_t buf_len, int hash_size) {

    Mat im = imdecode(Mat(1, buf_len, CV_8UC1, buf), IMREAD_GRAYSCALE);
    resize(im, im, Size(hash_size, hash_size), 0, 0, INTER_AREA);

    double avg = mean(im).val[0];

    auto *hash = new std::vector<bool>();

    uchar *pixel = im.ptr(0);
    int endPixel = im.cols * im.rows;
    for (int i = 0; i <= endPixel; i++) {
        hash->push_back(pixel[i] > avg);
    }

//    printBitSet(hash);
    delete hash;
}

void dhash(void *buf, size_t buf_len, int hash_size) {

    Mat im = imdecode(Mat(1, buf_len, CV_8UC1, buf), IMREAD_GRAYSCALE);
    resize(im, im, Size(hash_size + 1, hash_size), 0, 0, INTER_AREA);

    auto *hash = new std::vector<bool>();

    for (int i = 0; i < im.rows; ++i) {
        uchar *pixel = im.ptr(i);

        for (int j = 1; j < im.cols; ++j) {
            hash->push_back(pixel[j] > pixel[j - 1]);
        }
    }

//    printBitSet(hash);
    delete hash;
}

void whash(void *buf, size_t buf_len, int hash_size, int img_scale) {

    Mat im = imdecode(Mat(1, buf_len, CV_8UC1, buf), IMREAD_GRAYSCALE);

    if ((hash_size & (hash_size - 1)) != 0) {
        throw std::invalid_argument("hash_size must be a power of two");
    }

    if (img_scale != 0) {
        if ((img_scale & (img_scale - 1)) != 0) {
            throw std::invalid_argument("img_scale must be a power of two");
        }
    } else {
        int image_natural_scale = (int) pow(2, (int)log2(MIN(im.rows, im.cols)));
        img_scale = MAX(image_natural_scale, hash_size);
    }

    int ll_max_level = (int) log2(img_scale);
    int level = (int) log2(hash_size);

    if (ll_max_level < level) {
        throw std::invalid_argument("hash_size in a wrong range");
    }


    int dwt_level = ll_max_level - level;

    resize(im, im, Size(img_scale, img_scale), 0, 0, INTER_AREA);

    auto data = (double *) malloc(img_scale * img_scale * sizeof(double));

    uchar *pixel = im.ptr(0);
    const int endPixel = im.cols * im.rows;
    for (int i = 0; i <= endPixel; i++) {
        data[i] = (double) pixel[i] / 255;
    }

    wave_object w = wave_init("haar");
    wt2_object wt = wt2_init(w, "dwt", img_scale, img_scale, dwt_level);

    double *coeffs = dwt2(wt, data);
    free(data);

    double sorted[64];
    memcpy(sorted, coeffs, sizeof(double) * 64);

    double med = median(sorted, 64);
    auto *hash = new std::vector<bool>();

    for (int i = 0; i < hash_size * hash_size; ++i) {
        hash->push_back(coeffs[i] > med);
    }
//    printBitSet(hash);
    delete hash;
}

void phash(void *buf, size_t buf_len, int hash_size, int highfreq_factor) {

    int img_size = hash_size * highfreq_factor;

    Mat im = imdecode(Mat(1, buf_len, CV_8UC1, buf), IMREAD_GRAYSCALE);
    resize(im, im, Size(img_size, img_size), 0, 0, INTER_AREA);

    double pixels[img_size * img_size];

    uchar *pixel = im.ptr(0);
    int endPixel = im.cols * im.rows;
    for (int i = 0; i <= endPixel; i++) {
        pixels[i] = (double) pixel[i] / 255;
    }

    double out[img_size * img_size];
    fftw_plan plan = fftw_plan_r2r_2d(
            img_size, img_size,
            pixels, out,
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
            dct_lowfreq[ptr_low] = out[ptr];
            sorted[ptr_low] = out[ptr];
            ptr_low += 1;
            ptr += 1;
        }
        ptr += (img_size - hash_size);
    }

    double med = median(sorted, hash_size * hash_size);

    auto *hash = new std::vector<bool>();

    for (int i = 0; i < hash_size * hash_size; ++i) {
        hash->push_back(dct_lowfreq[i] > med);
    }

//    printBitSet(hash);
    delete hash;
}

