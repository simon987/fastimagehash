## libfastimagehash

[![CodeFactor](https://www.codefactor.io/repository/github/simon987/fastimagehash/badge/master)](https://www.codefactor.io/repository/github/simon987/fastimagehash/overview/master)
[![GitHub license](https://img.shields.io/github/license/simon987/fastimagehash)](https://github.com/simon987/fastimagehash/blob/master/LICENSE)

*WIP: Not ready for production use!*

`libfastimagehash` is a [imagehash](https://github.com/JohannesBuchner/imagehash/) 
replacement for C/C++.

**See also:**

 * [go bindings](https://github.com/simon987/fastimagehash-go)
(with *cgo*).


<p align="center">
  <img src="bench/results/phash_large.png"/>
</p>

### Example usage

```C
#include <fastimagehash.h>

int main() {
    unsigned char result[HASH_SIZE];

    phash_file("image.jpeg", result, HASH_SIZE, HIGHFREQ_FACTOR);
}
```

For slight additional performance gains, `libfastimagehash` can
compute all hashes at once instead of decoding the same 
image at each step.
<p align="center">
  <img src="bench/results/multi_small.png"/>
</p>

*[\*See all benchmarks](bench/)*


### Build from source

```bash
# Download dependencies
apt install libopencv-dev libfftw3-dev cmake

# Checkout source
git clone --recursive https://github.com/simon987/fastimagehash

# Build
cmake .
make

# Install as a system library
sudo make install
```


**Built with** 
* [opencv](https://github.com/opencv) for image decoding & resizing
* [wavelib](https://github.com/rafat/wavelib) for wavelet decomposition in `whash`  (With [additional CFLAGS](https://github.com/simon987/wavelib/commit/8d05b0b7321271b05365d3e89bfb0fd5a26d68a6))
* [fftw3](http://fftw.org/) for DCT in `phash`
* [google/benchmark](https://github.com/google/benchmark) to measure performance

**Disclaimer**: The methods used to compute the image hashes
are identical to the `imagehash` python library, however, due to
some slight differences in the way OpenCV vs Pillow images are resized,
the final image hashes are not always exactly the same. 
