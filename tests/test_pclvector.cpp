#include <catch2/catch.hpp>
#include <vector>
#include "PCLlib/pclvector.h"

// using IntVec = PCLvector_t(int);
// typedef PCLVector_t(int) IntVec;

using IntVec = PCLvector;

TEST_CASE("Integer vector push")
{
    constexpr int N = 256;
    IntVec v = PCLvector_create();

    REQUIRE(PCLvector_size(v) == 0);
    REQUIRE(PCLvector_empty(v) == true);
    REQUIRE(PCLvector_capacity(v) == 0);

    for (int i = 0; i < N; ++i) {
        PCLvector_push(v, i);
    }
    REQUIRE(PCLvector_size(v) == N);
    REQUIRE(PCLvector_empty(v) == false);

    for (int i = 0; i < N; ++i) {
        REQUIRE(PCLvector_A(v, i) == i);
    }

    for (int i = N - 1; i >= 0; --i) {
        REQUIRE(PCLvector_empty(v) == false);
        REQUIRE(PCLvector_pop(v) == i);
    }
    REQUIRE(PCLvector_empty(v) == true);

    PCLvector_finalize(v);
}
