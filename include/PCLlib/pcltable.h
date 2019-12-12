#ifndef PCLTABLE__H_
#define PCLTABLE__H_

#include <stdlib.h>

#ifndef PCL_assert
#include <assert.h>
#define PCL_assert(x) assert(x)
#endif

#ifndef PCL_roundup32
#define PCL_roundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))
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

enum PCL_Error {
    PCL_EMEM = 1,
};

using PCL_key = int;
using PCL_val = int;

struct PCL_table_s {
    PCL_key  *keys;
    PCL_val  *vals;
    uint32_t *flags;
    uint32_t  size;
    uint32_t  asize;
};
typedef struct PCL_table_s PCL_table;

struct PCL_iter_s {
    uint32_t v;
};
typedef struct PCL_iter_s PCL_iter;

PCL_table *PCL_create();
int PCL_init(PCL_table *t);
int PCL_init2(PCL_table *t, size_t minsize);
void PCL_destroy(PCL_table *t);
int PCL_clear(PCL_table *t);
int PCL_resize(PCL_table *t, size_t newsize);

PCL_table *PCL_create()
{
    PCL_table *t = (PCL_table*)PCL_realloc(NULL, sizeof(PCL_table));
    if (PCL_init(t) != 0) {
        PCL_free(t, sizeof(PCL_table));
        return NULL;
    }
    return t;
}

int PCL_init(PCL_table *t)
{
    PCL_init2(t, 8);
}

int PCL_init2(PCL_table *t, size_t minsize)
{
    t->size = 0;
    t->asize = PCL_roundup32(minsize);
    PCL_assert(t->asize % 2 == 0);
    t->keys = (PCL_key*)PCL_reallocarray(NULL, t->asize, sizeof(PCL_key));
    t->vals = (PCL_val*)PCL_reallocarray(NULL, t->asize, sizeof(PCL_val));
    if (!t->keys || !t->vals) {
        PCL_destroy(t);
        return -PCL_EMEM;
    }
    return 0;
}

void PCL_destroy(PCL_table *t)
{
    PCL_freearray(t->keys, t->size, sizeof(PCL_key));
    PCL_freearray(t->vals, t->size, sizeof(PCL_val));
#ifndef NDEBUG
    t->size = t->asize = 0;
#endif
}

int PCL_clear(PCL_table *t)
{
    return PCL_init2(t, 8);
}

int PCL_resize(PCL_table *t, size_t newsize)
{
    PCL_key* keys;
    PCL_val* vals;
    if (newsize <= t->asize)
        return 0;
    newsize = PCL_roundup32(newsize);
    keys = (PCL_key*)PCL_reallocarray(t->keys, newsize, sizeof(PCL_key));
    vals = (PCL_val*)PCL_reallocarray(t->vals, newsize, sizeof(PCL_val));
    if (!keys && !vals) {
        PCL_freearray(keys, newsize, sizeof(PCL_key));
        PCL_freearray(vals, newsize, sizeof(PCL_val));
        return 1;
    }
    t->keys = keys;
    t->vals = vals;
    t->asize = newsize;
    return 0;
}

/*! @function
 *  @abstract  Insert a key into the hash table.
 *  @param t   Pointer to the hash table. [PCL_table*]
 *  @param k   Key to insert.             [PCL_key]
 *  @param rc  Extra return code: -1 if operation failed;
 *             0 if the key is present in the table;
 *             1 if the bucket is empty;
 *             2 if the element was previously deleted; [int*]
 *  @return    Iterator to the inserted element [PCL_iter]
*/
// PCL_iter PCL_put(PCL_table *t, PCL_key k, int rc)
// {
// }

#define PCL_val(t, it) (*t).vals[(it).v]
#define PCL_deref(t, it) PCL_val(t, it)

#define PCL_size(t) (*t).size
#define PCL_asize(t) (*t).asize
#define PCL_capacity(t) PCL_asize(t)

#endif
