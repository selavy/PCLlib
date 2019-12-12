#include <catch2/catch.hpp>
#include "PCLlib/pcltable.h"

TEST_CASE("Round to power of 2")
{
    uint32_t x = 7;
    uint32_t y = PCL_roundup32(x);
    REQUIRE(y == 8);
}

TEST_CASE("Create table")
{
    PCL_table *t = PCL_create();
    REQUIRE(t != NULL);
    REQUIRE(PCL_size(t) == 0);
    REQUIRE(PCL_asize(t) == 0);
    REQUIRE(PCL_capacity(t) == 0);
    PCL_destroy(t);
}

TEST_CASE("Insert + Lookup")
{
    int rc;
    PCL_iter it;
    PCL_table *t;

    t = PCL_create();
    REQUIRE(t != NULL);
    REQUIRE(PCL_empty(t) == true);

    // it = PCL_put(t, 1, &rc);
    // REQUIRE(rc == 0);
    // REQUIRE(PCL_key(it) == 1);
    // PCL_val(it) = 2;
    // REQUIRE(PCL_size(t) == 1);

    PCL_destroy(t);
}
