#include <catch2/catch.hpp>
#include "PCLlib/pclarray.h"
#include <random>
#include <vector>

typedef PCLarray_t(int) IntArray;
using F64Array = PCLarray_t(double);

TEST_CASE("Array push")
{
    IntArray a = PCLarray_create();

    REQUIRE(PCLarray_size(a) == 0);
    REQUIRE(PCLarray_asize(a) == 0);
    REQUIRE(PCLarray_empty(a) == true);

    PCLarray_resize(a, 256);
    REQUIRE(PCLarray_asize(a) == 256);
    REQUIRE(PCLarray_size(a) == 0);

    const int N = 10;
    for (int i = 0; i < N; ++i) {
        PCLarray_push(a, i);
    }

    REQUIRE(PCLarray_size(a) == N);
    for (int i = 0; i < N; ++i) {
        REQUIRE(PCLarray_A(a, i) == i);
    }

    int v = PCLarray_pop(a);
    REQUIRE(v == N - 1);
    REQUIRE(PCLarray_size(a) == N - 1);

    v = PCLarray_pop(a);
    REQUIRE(v == N - 2);
    REQUIRE(PCLarray_size(a) == N - 2);
    REQUIRE(PCLarray_empty(a) == false);

    PCLarray_destroy(a);
}

TEST_CASE("Array growth")
{
    IntArray b = PCLarray_create();

    constexpr int N = 16;
    struct {
        int size;
        int asize;
    } expects[N] = {
        { 1, 1 },
        { 2, 3 },
        { 3, 3 },
        { 4, 7 },
        { 5, 7 },
        { 6, 7 },
        { 7, 7 },
        { 8, 15 },
        { 9, 15 },
        { 10, 15 },
        { 11, 15 },
        { 12, 15 },
        { 13, 15 },
        { 14, 15 },
        { 15, 15 },
        { 16, 31 },
    };

    for (int i = 0; i < N; ++i) {
        PCLarray_push(b, i);
        REQUIRE(PCLarray_size(b) == expects[i].size);
        REQUIRE(PCLarray_asize(b) == expects[i].asize);
    }
    REQUIRE(PCLarray_size(b) == N);

    for (int i = 0; i < N; ++i) {
        REQUIRE(PCLarray_A(b, i) == i);
    }

    PCLarray_destroy(b);
}

TEST_CASE("Array copy")
{
    SECTION("Copy empty array to empty array")
    {
        IntArray a = PCLarray_create();
        IntArray b = PCLarray_create();

        REQUIRE(PCLarray_empty(a) == true);
        REQUIRE(PCLarray_empty(b) == true);

        PCLarray_copy(b, a);

        REQUIRE(PCLarray_empty(a) == true);
        REQUIRE(PCLarray_empty(b) == true);

        PCLarray_destroy(b);
        PCLarray_destroy(a);
    }

    SECTION("Copy empty array to non-empty array")
    {
        IntArray a = PCLarray_create();
        IntArray b = PCLarray_create();

        const int N = 16;
        for (int i = 0; i < N; ++i) {
            PCLarray_push(b, i);
        }
        REQUIRE(PCLarray_size(b) == N);

        for (int i = 0; i < N; ++i) {
            REQUIRE(PCLarray_A(b, i) == i);
        }

        PCLarray_copy(b, a);

        REQUIRE(PCLarray_empty(b));
        REQUIRE(PCLarray_size(b) == PCLarray_size(a));

        PCLarray_destroy(b);
        PCLarray_destroy(a);
    }
}
