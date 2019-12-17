#include "PCLlib/linear_open_address.h"
#include <catch2/catch.hpp>

TEST_CASE("Constructor")
{
    loa_map<int, int> m;
    REQUIRE(m.size() == 0u);
    REQUIRE(m.capacity() == 0u);

    decltype(m)::iterator it = m.putkey(42);
    REQUIRE(m.size() == 1u);
    REQUIRE(m.capacity() >= 1u);
    REQUIRE(it.key() == 42);

    auto it2 = m.insert(43, 44);
    REQUIRE(m.size() == 2u);
    REQUIRE(m.capacity() >= 2u);
    REQUIRE(it2.key() == 43);
    REQUIRE(it2.value() == 44);
}
