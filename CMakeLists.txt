cmake_minimum_required(VERSION 3.10)
project(fastimagehash)


set(BUILD_UT OFF CACHE INTERNAL "")
add_subdirectory(thirdparty/wavelib)

set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE INTERNAL "")
set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "")
set(BENCHMARK_ENABLE_INSTALL OFF CACHE INTERNAL "")
set(CMAKE_BUILD_TYPE RELEASE CACHE INTERNAL "")
add_subdirectory(thirdparty/benchmark)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "cmake/" CACHE INTERNAL "")

find_package(OpenCV REQUIRED)
find_package(FFTW REQUIRED)

add_library(
        fastimagehash
        SHARED
        fastimagehash.cpp fastimagehash.h
)

target_include_directories(
        fastimagehash
        PUBLIC
        ${CMAKE_SOURCE_DIR}/thirdparty/wavelib/header/
        ${OpenCV_INCLUDE_DIRS}
        ${FFTW_INCLUDE_DIRS}
)

target_link_libraries(
        fastimagehash
        ${OpenCV_LIBS}
        ${FFTW_LIBRARIES}
        wavelib
        pthread
)


target_compile_options(
        fastimagehash
        PRIVATE
        -fPIC
        -Ofast
        -march=native
        -fno-stack-protector
        -fomit-frame-pointer
        -freciprocal-math
)

add_executable(bm benchmark.cpp)
target_link_libraries(
        bm
        fastimagehash
        benchmark
)
set_target_properties(
        bm
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bench/"
)

add_executable(imhash imhash.c)
target_link_libraries(
        imhash
        ${CMAKE_SOURCE_DIR}/libfastimagehash.so
)
target_compile_options(
        imhash
        PRIVATE
        -g
)

set_target_properties(fastimagehash PROPERTIES PUBLIC_HEADER "fastimagehash.h")
INSTALL(
        TARGETS fastimagehash
        LIBRARY DESTINATION /usr/lib/
        PUBLIC_HEADER DESTINATION /usr/include/
)

add_dependencies(fastimagehash wavelib)
add_dependencies(bm fastimagehash)
add_dependencies(bm benchmark)
add_dependencies(imhash fastimagehash)
