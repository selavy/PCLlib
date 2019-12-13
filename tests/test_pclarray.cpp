#include "PCLlib/pclarray.h"
#include <catch2/catch.hpp>
#include <random>
#include <vector>

TEST_CASE("Array push")
{
    PCLarray a = PCLarray_create();

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
    REQUIRE(v == N-1);
    REQUIRE(PCLarray_size(a) == N-1);

    v = PCLarray_pop(a);
    REQUIRE(v == N-2);
    REQUIRE(PCLarray_size(a) == N-2);
    REQUIRE(PCLarray_empty(a) == false);

    PCLarray_destroy(a);
}
