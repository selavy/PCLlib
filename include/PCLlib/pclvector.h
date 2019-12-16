#ifndef PCL_VECTOR__H_
#define PCL_VECTOR__H_

/*
 * TODO(selavy):
 *  + PCLvector_erase(v, i) -> memmove
 *  + PCLvector_eraserange(v, i1, i2) -> memmove
 *  + get rid of PCLvector_create(). Too confusing to use?
 */

#include <stdlib.h>
#include <string.h>

#ifndef PCL_reallocarray
#ifdef reallocarray
#define PCL_reallocarray(ptr, nmemb, size) reallocarray(ptr, nmemb, size)
#else
#define PCL_reallocarray(ptr, nmemb, size) realloc(ptr, (nmemb) * (size))
#endif
#endif

#ifndef PCL_freearray
#define PCL_freearray(ptr, nmemb, size) free(ptr)
#endif

#if __cplusplus >= 201103L
#define PCL_reallocarray2(ptr, nmemb, size)                                    \
    static_cast<decltype(ptr)>(reallocarray(ptr, nmemb, size))
#elif defined(__cplusplus)

#if defined(__GNUC__)
#define PCL_reallocarray2(ptr, nmemb, size)                                    \
    static_cast<__typeof__(ptr)>(reallocarray(ptr, nmemb, size))
#else
/* TODO(selavy): how to handle pre-c++11 without `typeof`? */
#error "Must use a C++ compiler with support for typeof or C++-11"
#endif

#else
#define PCL_reallocarray2 PCL_reallocarray
#endif

/* TODO(selavy): tune? */
#ifndef PCLVECTOR_DEFAULT_SIZE
#define PCLVECTOR_DEFAULT_SIZE 2
#endif

/* TODO(selavy): 1.5 or 2? */
#ifndef PCLVECTOR_GROWTH_FACTOR
#define PCLVECTOR_GROWTH_FACTOR 1.5
#endif

typedef int type;

#define PCLvector_t(type)                                                      \
    struct                                                                     \
    {                                                                          \
        int size;                                                              \
        int asize;                                                             \
        type* arr;                                                             \
    }

#define PCLvector_create()                                                     \
    {}
#define PCLvector_destroy(v)                                                   \
    do {                                                                       \
        PCL_freearray((v).arr, (v).asize, sizeof((v).arr[0]));                 \
        (v).arr = NULL;                                                        \
        (v).size = 0;                                                          \
        (v).asize = 0;                                                         \
    } while (0)
#define PCLvector_init(v) ((v).size = (v).asize = 0, (v).arr = NULL)
#define PCLvector_A(v, i) (v).arr[i]
#define PCLvector_size(v) ((v).size)
#define PCLvector_asize(v) ((v).asize)
#define PCLvector_capacity(v) PCLvector_asize(v)
#define PCLvector_empty(v) (PCLvector_size(v) == 0)
#define PCLvector_clear(v) PCLvector_destroy(v)
#define PCLvector_resize(v, newsize)                                           \
    do {                                                                       \
        (v).asize = newsize;                                                   \
        (v).arr = PCL_reallocarray2((v).arr, (v).asize, sizeof((v).arr[0]));   \
    } while (0)
#define PCLvector_reserve(v, newsize)                                          \
    do {                                                                       \
        if (newsize > (v).asize)                                               \
            PCL_resize(v, newsize);                                            \
    } while (0)
#define PCLvector_copy(dst, src)                                               \
    do {                                                                       \
        if ((dst).asize < (src).size)                                          \
            PCLvector_resize(dst, (src).size);                                 \
        (dst).size = (src).size;                                               \
        memcpy((dst).arr, (src).arr, sizeof((dst).arr[0]) * (src).size);       \
    } while (0)
#define PCLvector_push(v, x)                                                   \
    do {                                                                       \
        if ((v).size == (v).asize)                                             \
            PCLvector_resize(v, PCLvector__calcsize((v).asize));               \
        (v).arr[(v).size++] = x;                                               \
    } while (0)
#define PCLvector_pop(v) ((v).arr[--(v).size])
#define PCLvector_back(v) PCLvector_A(v, (v).size - 1)

/* private functions: */
#define PCLvector__calcsize(size)                                              \
    ((size) != 0 ? (size)*PCLVECTOR_GROWTH_FACTOR : PCLVECTOR_DEFAULT_SIZE)

#endif /* PCL_VECTOR__H_ */
