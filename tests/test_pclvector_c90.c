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
    CustomVec cmvec;

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
        PCLvector_init(cmvec);
        for (i = 0; i < N1; ++i) {
            custom_type ct;
            PCLvector_push(cmvec, ct);
            PCLvector_back(cmvec).x = 42 + i;
            PCLvector_back(cmvec).y = 43 + i;
            assert(PCLvector_size(cmvec) == i + 1);
        }
        assert(PCLvector_asize(cmvec) >= PCLvector_size(cmvec));
        for (i = 0; i < N2; ++i) {
            PCLvector_pop(cmvec);
        }
        assert(PCLvector_size(cmvec) == N1 - N2);
        for (i = 0; i < PCLvector_size(cmvec); ++i) {
            assert(PCLvector_A(cmvec, i).x == 42 + i);
            assert(PCLvector_A(cmvec, i).y == 43 + i);
        }
    }

    PCLvector_destroy(ivec);
    PCLvector_destroy(fvec);
    PCLvector_destroy(cvec);
    PCLvector_destroy(cmvec);
    printf("Passed.\n");

    return 0;
}
