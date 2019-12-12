#ifndef PCLTABLE__H_
#define PCLTABLE__H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PCL_assert
#include <assert.h>
#define PCL_assert(x) assert(x)
#endif

#ifndef PCL_roundup32
#define PCL_roundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))
#endif

#ifndef PCL_calloc
#define PCL_calloc(nmemb, size) calloc(nmemb, size)
#endif

#ifndef PCL_realloc
#define PCL_realloc(ptr, size) realloc(ptr, size)
#endif

#ifndef PCL_reallocarray
#define PCL_reallocarray(ptr, nmemb, size) reallocarray(ptr, nmemb, size)
#endif

#ifndef PCL_free
#define PCL_free(ptr, size) free(ptr)
#endif

#ifndef PCL_freearray
#define PCL_freearray(ptr, nmemb, size) free(ptr)
#endif


//
// Types
//

enum PCL_Error {
    PCL_EMEM = 1,
};

using PCL_key_t = int;
using PCL_val_t = int;

struct PCL_table_s {
    /* TODO: arrange so all one allocation? */
    PCL_key_t  *keys;
    PCL_val_t  *vals;
    uint8_t    *flags;  /* 2 bits each -- live, dead, tombstone */
    uint32_t    size;   /* # of live elements */
    uint32_t    asize;  /* # of array slots */
    uint32_t    used;   /* # of used slots */
    uint32_t    ubound; /* upper bound on used before rehash needed */
};
typedef struct PCL_table_s PCL_table;

struct PCL_iter_s {
    uint32_t v;
};
typedef struct PCL_iter_s PCL_iter;

struct PCL_kv_s {
    const PCL_key_t *key;
    PCL_val_t       *value;
};
typedef struct PCL_kv_s PCL_kv;

#define PCL__MinTableSize 4
/* TODO: tune? */
#define PCL__HashUpper 0.77
#define PCL__live(flags, i) ((flags[(i)/4] & (1u << (2*((i)%4)    ))) != 0)
#define PCL__tomb(flags, i) ((flags[(i)/4] & (1u << (2*((i)%4) + 1))) != 0)
#define PCL__animate(flags, i) do {             \
    flags[(i)/4] &= ~(1u << (2*((i) % 4) + 1)); \
    flags[(i)/4] |=  (1u << (2*((i) % 4)    )); \
} while (0)

//
// Declarations
//

PCL_table *PCL_create();
void PCL_init(PCL_table *t);
void PCL_destroy(PCL_table *t);
int PCL_clear(PCL_table *t);
int PCL_resize(PCL_table *t, size_t newsize);
#define PCL_end(t)       PCL_iter{t->asize}
#define PCL_key(t, it)   (*t).keys[(it).v]
#define PCL_val(t, it)   (*t).vals[(it).v]
#define PCL_value(t, it) PCL_val(t, it)
#define PCL_size(t) (*t).size
#define PCL_empty(t) (PCL_size(t) == 0)
#define PCL_asize(t) (*t).asize
#define PCL_capacity(t) PCL_asize(t)

/* TODO: implement */
uint32_t PCL__hash_func(PCL_key_t key)
{
    return key;
}

//
// Implementation
//

PCL_table *PCL_create()
{
    return (PCL_table*)PCL_calloc(1, sizeof(PCL_table));
}

void PCL_initialize(PCL_table *t)
{
    memset(t, 0, sizeof(*t));
}

void PCL_destroy(PCL_table *t)
{
    PCL_clear(t);
    PCL_free(t, sizeof(*t));
}

int PCL_clear(PCL_table *t)
{
    PCL_freearray(t->flags, t->size, sizeof(uint8_t));
    PCL_freearray(t->keys, t->size, sizeof(PCL_key_t));
    PCL_freearray(t->vals, t->size, sizeof(PCL_val_t));
    t->size = t->asize = t->used = 0;
    t->keys = NULL;
    t->vals = NULL;
}

#define PCL_finalize(t) PCL_clear(t)

int PCL_resize(PCL_table *t, size_t newsize)
{
    PCL_key_t* keys;
    PCL_val_t* vals;
    uint32_t* flags;
    if (newsize < PCL__MinTableSize || newsize <= t->asize)
        return 0;
    newsize = PCL_roundup32(newsize);
    flags = (uint32_t*)PCL_reallocarray(NULL, newsize, sizeof(uint8_t));
    keys = (PCL_key_t*)PCL_reallocarray(t->keys, newsize, sizeof(PCL_key_t));
    vals = (PCL_val_t*)PCL_reallocarray(t->vals, newsize, sizeof(PCL_val_t));
    if (!flags || !keys || !vals) {
        PCL_freearray(flags, newsize, sizeof(uint8_t));
        PCL_freearray(keys, newsize, sizeof(PCL_key_t));
        PCL_freearray(vals, newsize, sizeof(PCL_val_t));
        return -PCL_EMEM;
    }
    memset(flags, 0x0, newsize * sizeof(uint8_t));
    /* TODO: rehash if needed */
    t->keys = keys;
    t->vals = vals;
    t->asize = newsize;
    return 0;
}

/*! @function
 *  @abstract  Insert a key into the hash table.
 *  @param t   Pointer to the hash table. [PCL_table*]
 *  @param k   Key to insert.             [PCL_key_t]
 *  @param rc  Extra return code: -1 if operation failed;
 *             0 if the key is present in the table;
 *             1 if the bucket is empty;
 *             2 if the element was previously deleted; [int*]
 *  @return    Iterator to the inserted element [PCL_iter]
*/
// PCL_iter PCL_put(PCL_table *t, PCL_key_t k, int *rc)
// {
//     uint32_t i;
//     uint32_t mask;
//     uint32_t step;
//     uint32_t *flags;
//     const PCL_key_t *keys;
// 
//     if (t->used >= h->ubound) {
//         if (h->asize > 2*h->size) {
//             if ((*rc = PCL_resize(t, t->asize)) < 0)     /* clear tombstones */
//                 return PCL_end(t);
//         } else {
//             if ((*rc = PCL_resize(t, t->asize * 2)) < 0) /* grow table */
//                 return PCL_end(t);
//         }
//     }
// 
//     keys = t->keys;
//     flags = t->flags;
//     mask = t->asize - 1;
//     step = 1;
//     i = PCL__hash_func(key) & mask;
// 
//     // stop if:
//     // empty
//     // OR
//     // is deleted
//     // OR
//     // keys are equal
// 
//     for (;;) {
//         if (PCL__isempty(flags, i))
//             break;
//         if (PCL__isdel(flags, i))
//             break;
//         if (PCL__key_equal(k, keys[i]))
//             break;
//     }


//     flags = t->flags;
//     x = site = asize;
//     k = PCL__hash_func(key);
//     i = k & mask;
// 
    // if (PCL__isempty(flags, i))
    //     x = i;
    // else {
    //     last = i;
    //     while (!PCL__isempty(flags, i) && (PCL__isdel(flags, i) || !PCL__key_equal(h->keys[i], key))) {
    //         if (PCL__isdel(flags, i))
    //             site = i;
    //         i = (i + (++step)) & mask;
    //         if (i == last) {
    //             x = site;
    //             break;
    //         }
    //     }
    //     if (x == t->asize) {
    //         if (PCL__isempty(flags, i) && site != t->asize)
    //             x = site;
    //         else
    //             x = i;
    //     }
    // }
// }

PCL_kv PCL_deref(PCL_table *t, PCL_iter it)
{
    return { &t->keys[it.v], &t->vals[it.v] };
    /*
    PCL_kv kv;
    kv.key = &t->keys[it.v];
    kv.value = &t->vals[it.v];
    return kv;
    */
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
