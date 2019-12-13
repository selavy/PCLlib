#include <catch2/catch.hpp>
#include <vector>
#include <random>
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

TEST_CASE("Integer vector stress test")
{
    IntVec v1 = PCLvector_create();
    std::vector<int> v2;
    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < (1 << 14); ++i) {
        int x = gen();
        PCLvector_push(v1, x);
        v2.push_back(x);
        REQUIRE(PCLvector_size(v1) == v2.size());
        REQUIRE(PCLvector_back(v1) == v2.back());
    }

    REQUIRE(PCLvector_size(v1) == v2.size());
    for (int i = 0; i < (int)v2.size(); ++i) {
        REQUIRE(PCLvector_A(v1, i) == v2[i]);
    }

    enum Action {
        PUSH,
        POP,
        ACCESS,
        CHANGE,
        MAX_ACTION,
    };

    std::uniform_int_distribution<> dist(0, MAX_ACTION - 1);
    for (int i = 0; i < (1 << 14); ++i) {
        REQUIRE(PCLvector_size(v1) == v2.size());
        int action = dist(gen);
        if (action == PUSH) {
            int data = gen();
            PCLvector_push(v1, data);
            v2.push_back(data);
        } else if (action == POP) {
            if (!v2.empty()) {
                REQUIRE(PCLvector_back(v1) == v2.back());
                PCLvector_pop(v1);
                v2.pop_back();
                REQUIRE(PCLvector_back(v1) == v2.back());
            }
        } else if (action == ACCESS) {
            int i = gen() % v2.size();
            REQUIRE(PCLvector_A(v1, i) == v2[i]);
        } else if (action == CHANGE) {
            int data = gen();
            int i = gen() % v2.size();
            PCLvector_A(v1, i) = data;
            v2[i] = data;
        } else {
            REQUIRE(false); // update dist
        }
        REQUIRE(PCLvector_size(v1) == v2.size());
    }

    REQUIRE(PCLvector_size(v1) == v2.size());
    for (int i = 0; i < (int)v2.size(); ++i) {
        REQUIRE(PCLvector_A(v1, i) == v2[i]);
    }

    PCLvector_finalize(v1);
}

TEST_CASE("Vector copy")
{
    SECTION("Copy empty vector")
    {
        PCLvector a = PCLvector_create();
        PCLvector b = PCLvector_create();

        PCLvector_copy(a, b);
        REQUIRE(PCLvector_empty(a) == true);
        REQUIRE(PCLvector_size(a)  == 0);
    }

    SECTION("Copy empty vector to non-empty vector")
    {
        PCLvector a = PCLvector_create();
        PCLvector b = PCLvector_create();

        for (int i = 0; i < 256; ++i) {
            PCLvector_push(a, 1);
        }
        REQUIRE(PCLvector_empty(a) == false);

        PCLvector_copy(a, b);
        REQUIRE(PCLvector_empty(a) == true);
        REQUIRE(PCLvector_size(a)  == 0);
    }

    SECTION("Copy non-empty vector to empty vector")
    {
        PCLvector a = PCLvector_create();
        PCLvector b = PCLvector_create();

        REQUIRE(PCLvector_capacity(a) == 0);
        REQUIRE(PCLvector_empty(a) == true);

        for (int i = 0; i < 256; ++i) {
            PCLvector_push(b, 1);
        }
        REQUIRE(PCLvector_empty(b) == false);

        PCLvector_copy(a, b);
        REQUIRE(PCLvector_empty(a) == false);
        REQUIRE(PCLvector_size(a)  == PCLvector_size(b));

        for (int i = 0; i < PCLvector_size(b); ++i) {
            REQUIRE(PCLvector_A(a, i) == PCLvector_A(b, i));
        }
    }

    SECTION("Copy larger vector to non-empty vector")
    {
        PCLvector a = PCLvector_create();
        PCLvector b = PCLvector_create();

        for (int i = 0; i < 256; ++i) {
            PCLvector_push(a, 1);
        }
        for (int i = 0; i < 2056; ++i) {
            PCLvector_push(b, 2);
        }
        PCLvector_copy(a, b);

        REQUIRE(PCLvector_size(a) == PCLvector_size(b));
        for (int i = 0; i < PCLvector_size(b); ++i) {
            REQUIRE(PCLvector_A(a, i) == PCLvector_A(b, i));
        }
    }
}
