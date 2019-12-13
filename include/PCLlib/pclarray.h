#ifndef PCL_ARRAY__H_
#define PCL_ARRAY__H_

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

typedef int type;

struct PCLarray__header_s
{
    int size;
    int asize;
};
typedef PCLarray__header_s PCLarray__header;

typedef type* PCLarray;

#define PCLarray__h(v) ((PCLarray__header*)(v)-1)

/* TODO(selavy): maybe just always allocate header to remove NULL checks */
#define PCLarray_create() NULL
#define PCLarray_destroy(v) free((v) ? PCLarray__h(v) : NULL);
#define PCLarray_A(v, i) (v)[i]
#define PCLarray_size(v) ((v) ? PCLarray__h(v)->size : 0)
#define PCLarray_asize(v) ((v) ? PCLarray__h(v)->asize : 0)
#define PCLarray_capacity(v) PCLarray_asize(v)
#define PCLarray_empty(v) (PCLarray_size(v) == 0)
#define PCLarray_resize(v, newsize)                                            \
    do {                                                                       \
        PCLarray__header* h;                                                   \
        if (!v) {                                                              \
            h = PCL_realloc(                                                   \
              v, newsize * sizeof(*(v)) + sizeof(PCLarray__header));           \
            h->size = 0;                                                       \
        } else {                                                               \
            h =                                                                \
              PCL_realloc(PCLarray__h(v),                                      \
                          newsize * sizeof(*(v)) + sizeof(PCLarray__header));  \
        }                                                                      \
        h->asize = (newsize);                                                  \
        (v) = (typeof(v))(h + 1);                                              \
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

#endif // PCL_ARRAY__H_
