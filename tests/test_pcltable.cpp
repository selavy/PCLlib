#include <catch2/catch.hpp>
#include "PCLlib/pcltable.h"

TEST_CASE("Create table")
{
    PCL_table* t = PCL_create();
    REQUIRE(t != NULL);
    REQUIRE(PCL_size(t) == 0);
    REQUIRE(PCL_asize(t) > 0);
    REQUIRE(PCL_capacity(t) > 0);
    PCL_destroy(t);
}
