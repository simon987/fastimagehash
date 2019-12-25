#!/usr/bin/env bash

wget http://fftw.org/fftw-3.3.8.tar.gz
tar -xzf fftw-3.3.8.tar.gz
cd fftw-3.3.8
./configure CFLAGS=-fPIC
