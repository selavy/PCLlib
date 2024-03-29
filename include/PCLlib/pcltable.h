#ifndef PCLTABLE__H_
#define PCLTABLE__H_

#include <assert.h>
#include <errno.h>

#ifdef __cplusplus
#include <cstdint>
#include <cstdlib>
#include <cstring>
// extern "C" {
#else
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#endif

#ifndef PCL_roundup32
#define PCL_roundup32(x)                                                       \
    (--(x),                                                                    \
     (x) |= (x) >> 1,                                                          \
     (x) |= (x) >> 2,                                                          \
     (x) |= (x) >> 4,                                                          \
     (x) |= (x) >> 8,                                                          \
     (x) |= (x) >> 16,                                                         \
     ++(x))
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

#ifdef __cplusplus
#define PCL_cast(T, x) static_cast<T>(x)
#else
#define PCL_cast(T, x) x
#endif

using PCL_key_t = int;
using PCL_val_t = int;

struct PCL_table_s
{
    size_t size;   /* # of live elements */
    size_t asize;  /* # of array slots */
    size_t used;   /* # of used slots */
    size_t ubound; /* upper bound on used before rehash needed */
    /* TODO: arrange so all one allocation? */
    PCL_key_t* keys;
    PCL_val_t* vals;
    uint8_t* flags; /* 2 bits each -- live, dead, tombstone */
};
typedef struct PCL_table_s PCL_table;

struct PCL_iter_s
{
    size_t v;
};
typedef struct PCL_iter_s PCL_iter;

struct PCL_result_s
{
    PCL_iter iter;
    int rval; /* 0 = present, 1 = insert, 2 = insert (tombstone) */
};
typedef struct PCL_result_s PCL_result;

struct PCL_kv_s
{
    const PCL_key_t* key;
    PCL_val_t* value;
};
typedef struct PCL_kv_s PCL_kv;

PCL_table* PCLtable_create();
void PCLtable_init(PCL_table* t);
void PCLtable_destroy(PCL_table* t);
int PCLtable_clear(PCL_table* t);
int PCLtable_resize(PCL_table* t, size_t newsize);
int PCLtable_resize_fast(PCL_table* t, size_t newsize);
PCL_iter PCLtable_put(PCL_table *t, PCL_key_t k);
PCL_result PCLtable_put2(PCL_table *t, PCL_key_t k);
PCL_iter PCLtable_get(const PCL_table *t, PCL_key_t k);
PCL_kv PCLtable_deref(PCL_table* t, PCL_iter it);
int PCLtable_iter_eq(PCL_iter a, PCL_iter b);
int PCLtable_iter_neq(PCL_iter a, PCL_iter b);
PCL_iter PCLtable_iter_next(const PCL_table *t, PCL_iter it);
#define PCL__MinTableSize 8
#define PCL__HashLoadFactorUpperBound 0.77 /* TODO: tune */
#define PCL__live(flags, i) ((flags[(i) / 4] & (1u << (2 * ((i) % 4)))) != 0)
#define PCL__tomb(flags, i)                                                    \
    ((flags[(i) / 4] & (1u << (2 * ((i) % 4) + 1))) != 0)
#define PCL__animate(flags, i)                                                 \
    do {                                                                       \
        flags[(i) / 4] &= ~(1u << (2 * ((i) % 4) + 1));                        \
        flags[(i) / 4] |= (1u << (2 * ((i) % 4)));                             \
    } while (0)

#define PCLtable_end(t)                                                        \
    PCL_iter                                                                   \
    {                                                                          \
        t->asize                                                               \
    }
#define PCLtable_key(t, it) (*t).keys[(it).v]
#define PCLtable_val(t, it) (*t).vals[(it).v]
#define PCLtable_value(t, it) PCLtable_val(t, it)
#define PCLtable_size(t) (*t).size
#define PCLtable_empty(t) (PCLtable_size(t) == 0)
#define PCLtable_asize(t) (*t).asize
#define PCLtable_capacity(t) PCLtable_asize(t)
#define PCLtable_finalize(t) PCLtable_clear(t)

/*! @function
  @abstract     Test whether a bucket contains data.
  @param  h     Pointer to the hash table [khash_t(name)*]
  @param  x     Iterator to the bucket [khint_t]
  @return       1 if containing data; 0 otherwise [int]
 */
#define PCLtable_exists(t, x) PCL__live(t->flags, (x).v)

/* TODO: implement */
size_t PCL__hash_func(PCL_key_t key)
{
    return key;
}
int PCL__cmp_func(PCL_key_t a, PCL_key_t b)
{
    return a - b;
}

size_t (*hashfn)(PCL_key_t) = &PCL__hash_func;
int (*cmpfn)(PCL_key_t, PCL_key_t) = &PCL__cmp_func;

//
// Implementation
//

PCL_table* PCLtable_create()
{
    return PCL_cast(PCL_table*, PCL_calloc(1, sizeof(PCL_table)));
}

void PCLtable_init(PCL_table* t)
{
    memset(t, 0, sizeof(*t));
}

void PCLtable_destroy(PCL_table* t)
{
    PCLtable_clear(t);
    PCL_free(t, sizeof(*t));
}

int PCLtable_clear(PCL_table* t)
{
    PCL_freearray(t->flags, t->size, sizeof(uint8_t));
    PCL_freearray(t->keys, t->size, sizeof(PCL_key_t));
    PCL_freearray(t->vals, t->size, sizeof(PCL_val_t));
    t->size = t->asize = t->used = 0;
    t->keys = NULL;
    t->vals = NULL;
}

int PCLtable_resize(PCL_table* t, size_t newsize)
{
    if (newsize < PCL__MinTableSize || newsize <= t->asize)
        return 0;
    if (newsize * PCL__HashLoadFactorUpperBound <= t->size)
        return 0;
    PCL_roundup32(newsize);
    return PCLtable_resize_fast(t, newsize);
}

int PCLtable_resize_fast(PCL_table* t, size_t newsize)
{
    PCL_key_t *keys, *oldkeys = t->keys;
    PCL_val_t *vals, *oldvals = t->vals;
    uint8_t *flags, *oldflags = t->flags;
    size_t i, h, oldasize = t->asize, mask = newsize - 1;

    assert(newsize % 2 == 0);
    assert(newsize >= PCL__MinTableSize);
    assert(newsize * PCL__HashLoadFactorUpperBound > t->size);

    keys = (PCL_key_t*)PCL_reallocarray(NULL, newsize, sizeof(*keys));
    vals = (PCL_val_t*)PCL_reallocarray(NULL, newsize, sizeof(*vals));
    flags = (uint8_t*)PCL_calloc(newsize, sizeof(*flags));
    if (!keys || !vals || !flags) {
        PCL_freearray(keys, newsize, sizeof(*keys));
        PCL_freearray(vals, newsize, sizeof(*vals));
        PCL_freearray(flags, newsize, sizeof(*flags));
        return -ENOMEM;
    }

    for (i = 0; i < oldasize; ++i) {
        if (!PCL__live(oldflags, i))
            continue;
        h = hashfn(oldkeys[i]) & mask;
        for (; PCL__live(flags, h); h = (h + 1) & mask)
            ;
        assert(!PCL__live(flags, h));
        keys[h] = oldkeys[i];
        vals[h] = oldvals[i];
        PCL__animate(flags, h);
    }

    t->keys = keys;
    t->vals = vals;
    t->flags = flags;
    t->asize = newsize;
    t->ubound = newsize * PCL__HashLoadFactorUpperBound;
    free(oldkeys);
    free(oldvals);
    free(oldflags);
    return 0;
}

/*! @function
 *  @abstract  Insert a key into the hash table.
 *  @param t   Pointer to the hash table. [PCL_table*]
 *  @param k   Key to insert.             [PCL_key_t]
 *  @return    Iterator to the inserted element or end [PCL_iter]
 */
PCL_iter PCLtable_put(PCL_table *t, PCL_key_t k)
{
    return PCLtable_put2(t, k).iter;
}

/*! @function
 *  @abstract  Insert a key into the hash table.
 *  @param t   Pointer to the hash table. [PCL_table*]
 *  @param k   Key to insert.             [PCL_key_t]
 *  @return    Pair of iterator to the inserted element
 *             and an extra return code:
 *                 -1 if operation failed;
 *                 0 if the key is present in the table;
 *                 1 if the bucket is empty;
 *                 2 if the element was previously deleted; [PCL_result]
 */
PCL_result PCLtable_put2(PCL_table* t, PCL_key_t k)
{
    int rc;
    size_t mask, h;
    uint8_t* flags;
    PCL_key_t* keys;

    if (t->used + 1 >= t->ubound) {
        size_t newsize = t->asize == 0 ? PCL__MinTableSize : 2 * t->asize;
        if ((rc = PCLtable_resize_fast(t, newsize)) != 0)
            return { PCLtable_end(t), rc };
    }

    flags = t->flags;
    keys = t->keys;
    mask = t->asize - 1;
    h = hashfn(k) & mask;
    for (;;) {
        if (!PCL__live(flags, h)) {
            PCL__animate(flags, h);
            keys[h] = k;
            t->size++;
            t->used++;
            /* TODO(selavy): make the flag values match so can remove branch? */
            return { h, PCL__tomb(flags, h) ? 2 : 1 };
        } else if (cmpfn(keys[h], k) == 0) {
            return { h, 0 };
        }
        h = (h + 1) & mask;
    }
    __builtin_unreachable();
}

PCL_iter PCLtable_get(const PCL_table *t, PCL_key_t k)
{
    size_t h, mask = t->asize - 1;
    PCL_key_t *keys = t->keys;
    uint8_t *flags = t->flags;
    h = hashfn(k) & mask;
    for (;;) {
        if (PCL__live(flags, h)) {
            if (cmpfn(k, keys[h]) == 0)
                return { h };
        } else if (!PCL__tomb(flags, h)) {
            return { t->asize };
        }
    }
    __builtin_unreachable();
}

PCL_kv PCLtable_deref(PCL_table* t, PCL_iter it)
{
    return { &t->keys[it.v], &t->vals[it.v] };
    /*
    PCL_kv kv;
    kv.key = &t->keys[it.v];
    kv.value = &t->vals[it.v];
    return kv;
    */
}

int PCLtable_iter_eq(PCL_iter a, PCL_iter b)
{
    return (a.v - b.v) == 0;
}

int PCLtable_iter_neq(PCL_iter a, PCL_iter b)
{
    return (a.v - b.v) != 0;
}

PCL_iter PCLtable_iter_next(const PCL_table *t, PCL_iter it)
{
    uint8_t* flags = t->flags;
    size_t i;
    for (i = it.v; i != t->asize; ++i) {
        if (PCL__live(flags, i))
            return { i };
    }
    return { i };
}

// #ifdef __cplusplus
// } // extern "C"
// #endif

#endif
