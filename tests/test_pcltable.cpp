#include <catch2/catch.hpp>
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
    PCL_result res;
    PCL_table *t;
    const int N = 256;

    t = PCLtable_create();
    REQUIRE(t != NULL);
    REQUIRE(PCLtable_empty(t) == true);

    for (int i = 0; i < N; ++i) {
        INFO("Inserting " << i << "...");
        if (t->used + 1 >= t->ubound) {
            INFO("Resizing...");
        }
        res = PCLtable_put2(t, i);
        REQUIRE(res.rval == 1);
        REQUIRE(PCLtable_key(t, res.iter) == i);
        PCLtable_val(t, res.iter) = i + 100;
        REQUIRE(PCLtable_size(t) == i + 1);
    }
    REQUIRE(PCLtable_size(t) == N);
    REQUIRE(PCLtable_asize(t) > N);

    PCLtable_destroy(t);
}
