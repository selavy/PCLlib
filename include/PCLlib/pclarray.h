#ifndef PCL_ARRAY__H_
#define PCL_ARRAY__H_

#include <stdlib.h>
#include <string.h>

#ifndef PCL_realloc
#ifdef __cplusplus
#define PCL_realloc(ptr, size)                                                 \
    static_cast<PCLarray__header*>(realloc(ptr, size))
#else
#define PCL_realloc(ptr, size) realloc(ptr, size)
#endif
#endif

#ifndef PCL_freearray
#define PCL_freearray(ptr, nmemb, size) free(ptr)
#endif

struct PCLarray__header_s
{
    int size;
    int asize;
};
typedef PCLarray__header_s PCLarray__header;

#define PCLarray_t(type) type*

#define PCLarray__h(v) ((PCLarray__header*)(v)-1)
#define PCLarray__s(v) (v) ? PCLarray__h(v) : NULL

/* TODO(selavy): maybe just always allocate header to remove NULL checks */
#define PCLarray_create() NULL
#define PCLarray_destroy(v)                                                    \
    PCL_freearray(PCLarray__s(v), PCLarray_asize(v), sizeof(*v));
#define PCLarray_A(v, i) (v)[i]
#define PCLarray_size(v) ((v) ? PCLarray__h(v)->size : 0)
#define PCLarray_asize(v) ((v) ? PCLarray__h(v)->asize : 0)
#define PCLarray_capacity(v) PCLarray_asize(v)
#define PCLarray_empty(v) (PCLarray_size(v) == 0)
#define PCLarray__sz(v, size) (sizeof(PCLarray__header) + (size) * sizeof(*(v)))
#define PCLarray_resize(v, newsize)                                            \
    do {                                                                       \
        int osize = PCLarray_size(v);                                          \
        int nsize = (newsize);                                                 \
        PCLarray__header* h =                                                  \
          PCL_realloc(PCLarray__s(v), PCLarray__sz(v, nsize));                 \
        h->size = osize;                                                       \
        h->asize = nsize;                                                      \
        v = (typeof(v))(h + 1);                                                \
    } while (0)
/* TODO(selavy): tune growth */
#define PCLarray__newsize(v) (2 * PCLarray_asize(v) + 1)
#define PCLarray_push(v, x)                                                    \
    do {                                                                       \
        if (PCLarray_size(v) == PCLarray_asize(v))                             \
            PCLarray_resize(v, PCLarray__newsize(v));                          \
        (v)[PCLarray__h(v)->size++] = (x);                                     \
    } while (0)
#define PCLarray_pop(v) ((v)[--PCLarray__h(v)->size])
#define PCLarray_copy(dst, src)                                                \
    do {                                                                       \
        if (PCLarray_empty(dst)) {                                             \
            if (!PCLarray_empty(src)) {                                        \
                PCLarray_resize(dst, PCLarray_size(src));                      \
                memcpy(dst, src, sizeof(*dst) * PCLarray_size(src));           \
                PCLarray__h(dst)->size = PCLarray_size(src);                   \
            }                                                                  \
        } else if (PCLarray_empty(src)) {                                      \
            assert(!PCLarray_empty(dst));                                      \
            PCLarray__h(dst)->size = 0;                                        \
        } else {                                                               \
            if (PCLarray_asize(dst) < PCLarray_size(src))                      \
                PCLarray_resize(dst, PCLarray_size(src));                      \
            memcpy(dst, src, sizeof(*(dst)) * PCLarray_size(src));             \
            PCLarray__h(dst)->size = PCLarray_size(src);                       \
        }                                                                      \
    } while (0)

#endif // PCL_ARRAY__H_
