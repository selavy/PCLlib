#include <catch2/catch.hpp>
#include <random>
#include "PCLlib/pcltable.h"

TEST_CASE("Create table")
{
    PCL_table *t = PCLtable_create();
    REQUIRE(t != NULL);
    REQUIRE(PCLtable_size(t) == 0);
    REQUIRE(PCLtable_asize(t) == 0);
    REQUIRE(PCLtable_capacity(t) == 0);
    PCLtable_destroy(t);
}

TEST_CASE("Resize table")
{
    PCL_table *t = PCLtable_create();
    REQUIRE(t != NULL);
    REQUIRE(PCLtable_size(t) == 0);
    REQUIRE(PCLtable_asize(t) == 0);

    PCLtable_resize(t, 8);
    REQUIRE(PCLtable_asize(t) >= 8);

    PCLtable_resize(t, 24);
    REQUIRE(PCLtable_asize(t) >= 24);

    PCLtable_destroy(t);
}

TEST_CASE("Insert + Lookup")
{
    const int N = 256;
    PCL_table *t;
    PCL_result res;
    PCL_iter it;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist1(0, N);
    std::uniform_int_distribution<> dist2(N, 2*N);

    t = PCLtable_create();
    REQUIRE(t != NULL);
    REQUIRE(PCLtable_empty(t) == true);

    for (int i = 0; i < N; ++i) {
        res = PCLtable_put2(t, i);
        REQUIRE(res.rval == 1);
        REQUIRE(PCLtable_key(t, res.iter) == i);
        PCLtable_val(t, res.iter) = i + 100;
        REQUIRE(PCLtable_size(t) == i + 1);
    }
    REQUIRE(PCLtable_size(t) == N);
    REQUIRE(PCLtable_asize(t) > N);

    for (int i = 0; i < 10; ++i) {
        int v = dist1(gen);
        INFO("Looking up " << v);
        it = PCLtable_get(t, v);
        REQUIRE(PCLtable_iter_neq(it, PCLtable_end(t)));
        REQUIRE(PCLtable_key(t, it) == v);
        REQUIRE(PCLtable_val(t, it) == v + 100);
        REQUIRE(*PCLtable_deref(t, it).key == v);
        REQUIRE(*PCLtable_deref(t, it).value == v + 100);
    }

    for (int i = 0; i < 10; ++i) {
        int v = dist2(gen);
        INFO("Looking up " << v);
        it = PCLtable_get(t, v);
        REQUIRE(PCLtable_iter_eq(it, PCLtable_end(t)));
    }

    PCLtable_destroy(t);
}
