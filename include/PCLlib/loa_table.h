#ifndef NAME
#error "Must define table name in symbol `NAME`"
#endif

/*
 * NAME  - prefix for table
 * KEY   - key type
 * VALUE - value type
 * HASH  - hash function of form `size_t HASH(KEY key)`
 * KEYEQ - key comparison function of form `int KEYEQ(KEY k1, KEY k2)`,
 *         returning 0 if k1 == k2.
 */
#ifndef KEY
#error "Must define key type in symbol `KEY`"
#endif
#ifndef VALUE
#error "Must define value type in symbol `VALUE`"
#endif

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef STRCAT
#define STRCAT(s1, s2) s1##s2
#define STRCAT2(s1, s2) STRCAT(s1, s2)
#endif

#define freearray(ptr, nmemb, size) free(ptr)

#define pltable STRCAT2(NAME, table)
#define pltable_t STRCAT2(pltable, _t)
#define pltable_s STRCAT2(NAME, table_s)
#define PUBLIC_API(func) STRCAT2(STRCAT2(NAME, _), func)

#ifndef HASH
/* TODO: better default hash */
size_t HASH(KEY key) { return (size_t)key; }
#endif

#ifndef KEYEQ
int KEYEQ(KEY k1, KEY k2)
{
    return k1 - k2;
}
#endif

#define LOA_MAX_LOAD_FACTOR 0.77

struct pltable_t
{
    size_t size;
    size_t asize;
    size_t used;
    size_t limit;

    size_t* flgs;
    KEY* keys;
    VALUE* vals;
};
typedef struct pltable_t* pltable;

// 2 bits per entry in flags
// 0x0 = dead unused entry
// 0x1 = live entry
// 0x2 = dead used entry (aka tombstone)
#define loa__is_live(ms, i)                                                    \
    ((ms[(i) / sizeof(ms)] & (0x1u << 2 * ((i) % sizeof(ms)))) != 0)
#define loa__is_tomb(ms, i)                                                    \
    ((ms[(i) / sizeof(ms)] & (0x2u << 2 * ((i) % sizeof(ms)))) != 0)
#define loa__is_unused(ms, i)                                                  \
    ((ms[(i) / sizeof(ms)] & (0x3u << 2 * ((i) % sizeof(ms)))) == 0)
#define loa__animate(ms, i)                                                    \
    do {                                                                       \
        ms[(i) / sizeof(ms)] &= ~(1u << (2 * ((i) % sizeof(ms)) + 1));         \
        ms[(i) / sizeof(ms)] |= (1u << (2 * ((i) % sizeof(ms))));              \
    } while (0)
#define loa__fsize(asize) ((asize) / sizeof(size_t))

pltable PUBLIC_API(create)()
{
    return calloc(1, sizeof(pltable));
}

void PUBLIC_API(init)(pltable t)
{
    t->flgs = NULL;
    t->keys = NULL;
    t->vals = NULL;
    t->size = t->asize = t->used = t->limit = 0;
}

void PUBLIC_API(free)(pltable t)
{
    freearray(t->flgs, loa__fsize(t->asize), sizeof(*t->flgs));
    freearray(t->keys, t->asize            , sizeof(*t->keys));
    freearray(t->vals, t->asize            , sizeof(*t->vals));
    PUBLIC_API(init)(t);
}

void PUBLIC_API(destroy)(pltable t)
{
    PUBLIC_API(free)(t);
    free(t);
}

int PUBLIC_API(resize_unsafe)(pltable t, size_t newsize)
{
    size_t i, newidx, oldsize = t->asize, mask = newsize - 1;
    size_t *flgs, *oldflgs = t->flgs;
    KEY *keys, *oldkeys = t->keys;
    VALUE *vals, *oldvals = t->vals;

    assert(newsize & (newsize - 1) == 0); /* table size must be a power of 2 */
    assert(newsize > t->limit);

    flgs = calloc(loa__fsize(newsize), sizeof(*flgs));
    keys = calloc(newsize            , sizeof(*keys));
    vals = calloc(newsize            , sizeof(*vals));
    if (!flgs || !keys || !vals) {
        freearray(flgs, loa__fsize(newsize), sizeof(*flgs));
        freearray(keys, newsize            , sizeof(*keys));
        freearray(vals, newsize            , sizeof(*vals));
        return -ENOMEM;
    }

    /*
     * TODO(selavy): can I use reallocarray() instead and do something
     * like cuckoo hashing to rehash the elements?
     */
    for (i = 0; i < oldsize; ++i) {
        if (!loa__is_live(oldflgs, i))
            continue;
        newidx = HASH(oldkeys[i]) & mask;
        for (;;) {
            if (!loa__is_live(flgs, newidx)) {
                loa__animate(flgs, newidx);
                keys[newidx] = oldkeys[i];
                vals[newidx] = oldvals[i];
                break;
            }
            newidx = (newidx + 1) & mask;
        }
    }

    freearray(oldflgs, loa__fsize(oldasize), sizeof(*oldflgs));
    freearray(oldkeys, oldasize            , sizeof(*oldkeys));
    freearray(oldvals, oldasize            , sizeof(*oldvals));
    t->flgs = flgs;
    t->keys = keys;
    t->vals = vals;
    t->asize = newsize;
    t->used = t->size;
    t->limit = newsize * LOA_MAX_LOAD_FACTOR;

    return 0;
}

#undef NAME
#undef KEY
#undef VALUE
#undef STRCAT
#undef pltable
#undef pltable_t
#undef loa__is_live
#undef loa__is_tomb
#undef loa__is_unused
#undef loa__animate
#undef LOA_MAX_LOAD_FACTOR
