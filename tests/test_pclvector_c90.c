#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "PCLlib/pclvector.h"

typedef PCLvector_t(int) I32Vec;
typedef PCLvector_t(double) F64Vec;
typedef PCLvector_t(char) CharVec;

struct custom_type_s {
    int x;
    int y;
};
typedef struct custom_type_s custom_type;

typedef PCLvector_t(custom_type) CustomVec;

int ctcmp(custom_type a, custom_type b)
{
    return a.x == b.x && a.y == b.y ? 0 : 1;
}

int dblcmp(double x, double y)
{
    double eps = 0.0001;
    double diff = x - y;
    if (-eps < diff && diff < eps) {
        return 0;
    } else if (diff < 0) {
        return -1;
    } else {
        return 1;
    }
}

int main(int argc, char** argv)
{
    int i, N1 = 256, N2 = 128;
    I32Vec ivec;
    F64Vec fvec;
    CharVec cvec;
    CustomVec ctvec, src, dst;

    {
        PCLvector_init(ivec);
        for (i = 0; i < N1; ++i) {
            PCLvector_push(ivec, i);
            assert(PCLvector_back(ivec) == i);
            assert(PCLvector_size(ivec) == i + 1);
        }
        for (i = 0; i < N2; ++i) {
            PCLvector_pop(ivec);
        }
        assert(PCLvector_asize(ivec) >= PCLvector_size(ivec));
        assert(PCLvector_size(ivec) == N1 - N2);
        for (i = 0; i < PCLvector_size(ivec); ++i) {
            assert(PCLvector_A(ivec, i) == i);
        }
    }

    {
        PCLvector_init(fvec);
        for (i = 0; i < N1; ++i) {
            PCLvector_push(fvec, 42.0 + i);
            assert(dblcmp(PCLvector_back(fvec), 42.0 + i) == 0);
            assert(PCLvector_size(fvec) == i + 1);
        }
        assert(PCLvector_asize(fvec) >= PCLvector_size(fvec));
        for (i = 0; i < N2; ++i) {
            PCLvector_pop(fvec);
        }
        assert(PCLvector_size(fvec) == N1 - N2);
        for (i = 0; i < PCLvector_size(fvec); ++i) {
            assert(dblcmp(PCLvector_A(fvec, i), 42.0 + i) == 0);
        }
    }

    {
        PCLvector_init(cvec);
        for (i = 0; i < N1; ++i) {
            PCLvector_push(cvec, i % 128);
            assert(PCLvector_back(cvec) == i % 128);
            assert(PCLvector_size(cvec) == i + 1);
        }
        assert(PCLvector_asize(cvec) >= PCLvector_size(cvec));
        for (i = 0; i < N2; ++i) {
            PCLvector_pop(cvec);
        }
        assert(PCLvector_size(cvec) == N1 - N2);
        for (i = 0; i < PCLvector_size(cvec); ++i) {
            assert(PCLvector_A(cvec, i) == i % 128);
        }
    }

    {
        PCLvector_init(ctvec);
        for (i = 0; i < N1; ++i) {
            custom_type ct;
            PCLvector_push(ctvec, ct);
            PCLvector_back(ctvec).x = 42 + i;
            PCLvector_back(ctvec).y = 43 + i;
            assert(PCLvector_size(ctvec) == i + 1);
        }
        assert(PCLvector_asize(ctvec) >= PCLvector_size(ctvec));
        for (i = 0; i < N2; ++i) {
            PCLvector_pop(ctvec);
        }
        assert(PCLvector_size(ctvec) == N1 - N2);
        for (i = 0; i < PCLvector_size(ctvec); ++i) {
            assert(PCLvector_A(ctvec, i).x == 42 + i);
            assert(PCLvector_A(ctvec, i).y == 43 + i);
        }
    }

    PCLvector_init(src);
    PCLvector_init(dst);

    { // empty to empty
        PCLvector_clear(dst);
        PCLvector_clear(src);
        PCLvector_copy(dst, src);
        assert(PCLvector_empty(dst));
    }

    { // empty to non-empty
        PCLvector_clear(dst);
        PCLvector_clear(src);
        for (i = 0; i < 22; ++i) {
            custom_type ct = { i , i + 1 };
            PCLvector_push(dst, ct);
        }
        PCLvector_copy(dst, src);
        assert(PCLvector_empty(dst));
    }


    { // non-empty to empty
        PCLvector_clear(dst);
        PCLvector_clear(src);
        for (i = 0; i < 234; ++i) {
            custom_type ct = { 42 + i, 44 + i};
            PCLvector_push(src, ct);
        }
        PCLvector_copy(dst, src);
        assert(PCLvector_size(dst) == PCLvector_size(src));
        for (i = 0; i < PCLvector_size(dst); ++i) {
            assert(ctcmp(PCLvector_A(dst, i), PCLvector_A(src, i)) == 0);
        }
    }

    { // smaller non-empty to non-empty
        PCLvector_clear(dst);
        PCLvector_clear(src);
        for (i = 0; i < 22; ++i) {
            custom_type ct = { 42 + i, 44 + i};
            PCLvector_push(src, ct);
        }
        for (i = 0; i < 1024; ++i) {
            custom_type ct = { i, i + 1 };
            PCLvector_push(dst, ct);
        }
        PCLvector_copy(dst, src);
        assert(PCLvector_size(dst) == PCLvector_size(src));
        for (i = 0; i < PCLvector_size(dst); ++i) {
            assert(ctcmp(PCLvector_A(dst, i), PCLvector_A(src, i)) == 0);
        }
    }

    { // larger non-empty to non-empty
        PCLvector_clear(dst);
        PCLvector_clear(src);
        for (i = 0; i < 1024; ++i) {
            custom_type ct = { i, i + 1 };
            PCLvector_push(src, ct);
        }
        for (i = 0; i < 22; ++i) {
            custom_type ct = { 42 + i, 44 + i};
            PCLvector_push(dst, ct);
        }
        PCLvector_copy(dst, src);
        assert(PCLvector_size(dst) == PCLvector_size(src));
        for (i = 0; i < PCLvector_size(dst); ++i) {
            assert(ctcmp(PCLvector_A(dst, i), PCLvector_A(src, i)) == 0);
        }
    }

    PCLvector_destroy(ivec);
    PCLvector_destroy(fvec);
    PCLvector_destroy(cvec);
    PCLvector_destroy(ctvec);
    PCLvector_destroy(src);
    PCLvector_destroy(dst);
    printf("Passed.\n");

    return 0;
}
