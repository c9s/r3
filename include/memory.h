/*
 * Copyright (c) 2014,2015 DeNA Co., Ltd., Kazuho Oku, Justin Zhu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#ifndef r3__memory_h
#define r3__memory_h

#ifdef __sun__
#include <alloca.h>
#endif
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#ifdef __cplusplus
extern "C" {
#endif

#define R3_STRUCT_FROM_MEMBER(s, m, p) ((s *)((char *)(p)-offsetof(s, m)))

#if __GNUC__ >= 3
#define R3_LIKELY(x) __builtin_expect(!!(x), 1)
#define R3_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define R3_LIKELY(x) (x)
#define R3_UNLIKELY(x) (x)
#endif

#ifdef __GNUC__
#define R3_GNUC_VERSION ((__GNUC__ << 16) | (__GNUC_MINOR__ << 8) | __GNUC_PATCHLEVEL__)
#else
#define R3_GNUC_VERSION 0
#endif

#if __STDC_VERSION__ >= 201112L
#define R3_NORETURN _Noreturn
#elif defined(__clang__) || defined(__GNUC__) && R3_GNUC_VERSION >= 0x20500
// noreturn was not defined before gcc 2.5
#define R3_NORETURN __attribute__((noreturn))
#else
#define R3_NORETURN
#endif

#if !defined(__clang__) && defined(__GNUC__) && R3_GNUC_VERSION >= 0x40900
// returns_nonnull was seemingly not defined before gcc 4.9 (exists in 4.9.1 but not in 4.8.2)
#define R3_RETURNS_NONNULL __attribute__((returns_nonnull))
#else
#define R3_RETURNS_NONNULL
#endif

typedef struct st_r3_buffer_prototype_t r3_buffer_prototype_t;

/**
 * buffer structure compatible with iovec
 */
typedef struct st_r3_iovec_t {
    char *base;
    unsigned int len;
} r3_iovec_t;

typedef struct st_r3_mem_recycle_t {
    unsigned int max;
    unsigned int cnt;
    struct st_r3_mem_recycle_chunk_t *_link;
} r3_mem_recycle_t;

struct st_r3_mem_pool_shared_entry_t {
    unsigned int refcnt;
    void (*dispose)(void *);
    char bytes[1];
};

/**
 * the memory pool
 */
typedef struct st_r3_mem_pool_t {
    struct st_r3_mem_pool_chunk_t *chunks;
    unsigned int chunk_offset;
    struct st_r3_mem_pool_shared_ref_t *shared_refs;
    struct st_r3_mem_pool_direct_t *directs;
} r3_mem_pool_t;

/**
 * buffer used to store incoming / outgoing octets
 */
typedef struct st_r3_buffer_t {
    /**
     * capacity of the buffer (or minimum initial capacity in case of a prototype (i.e. bytes == NULL))
     */
    unsigned int capacity;
    /**
     * amount of the data available
     */
    unsigned int size;
    /**
     * pointer to the start of the data (or NULL if is pointing to a prototype)
     */
    char *bytes;
    /**
     * prototype (or NULL if the instance is part of the prototype (i.e. bytes == NULL))
     */
    r3_buffer_prototype_t *_prototype;
    /**
     * file descriptor (if not -1, used to store the buffer)
     */
    int _fd;
    char _buf[1];
} r3_buffer_t;

typedef struct st_r3_buffer_mmap_settings_t {
    unsigned int threshold;
    char fn_template[FILENAME_MAX];
} r3_buffer_mmap_settings_t;

struct st_r3_buffer_prototype_t {
    r3_mem_recycle_t allocator;
    r3_buffer_t _initial_buf;
    r3_buffer_mmap_settings_t *mmap_settings;
};

#define R3_VECTOR(type)                                                                                                           \
    struct {                                                                                                                       \
        type *entries;                                                                                                             \
        unsigned int size;                                                                                                               \
        unsigned int capacity;                                                                                                           \
    }

typedef R3_VECTOR(void) r3_vector_t;

extern void *(*r3_mem__set_secure)(void *, int, unsigned int);

/**
 * prints an error message and aborts
 */
R3_NORETURN void r3_fatal(const char *msg);

/**
 * constructor for r3_iovec_t
 */
static r3_iovec_t r3_iovec_init(const void *base, unsigned int len);
/**
 * wrapper of malloc; allocates given size of memory or dies if impossible
 */
R3_RETURNS_NONNULL static void *r3_mem_alloc(unsigned int sz);
/**
 * warpper of realloc; reallocs the given chunk or dies if impossible
 */
static void *r3_mem_realloc(void *oldp, unsigned int sz);

/**
 * allocates memory using the reusing allocator
 */
void *r3_mem_alloc_recycle(r3_mem_recycle_t *allocator, unsigned int sz);
/**
 * returns the memory to the reusing allocator
 */
void r3_mem_free_recycle(r3_mem_recycle_t *allocator, void *p);

/**
 * initializes the memory pool.
 */
void r3_mem_init_pool(r3_mem_pool_t *pool);
/**
 * clears the memory pool.
 * Applications may dispose the pool after calling the function or reuse it without calling r3_mem_init_pool.
 */
void r3_mem_clear_pool(r3_mem_pool_t *pool);
/**
 * allocates given size of memory from the memory pool, or dies if impossible
 */
void *r3_mem_alloc_pool(r3_mem_pool_t *pool, unsigned int sz);
/**
 * allocates a ref-counted chunk of given size from the memory pool, or dies if impossible.
 * The ref-count of the returned chunk is 1 regardless of whether or not the chunk is linked to a pool.
 * @param pool pool to which the allocated chunk should be linked (or NULL to allocate an orphan chunk)
 */
void *r3_mem_alloc_shared(r3_mem_pool_t *pool, unsigned int sz, void (*dispose)(void *));
/**
 * links a ref-counted chunk to a memory pool.
 * The ref-count of the chunk will be decremented when the pool is cleared.
 * It is permitted to link a chunk more than once to a single pool.
 */
void r3_mem_link_shared(r3_mem_pool_t *pool, void *p);
/**
 * increments the reference count of a ref-counted chunk.
 */
static void r3_mem_addref_shared(void *p);
/**
 * decrements the reference count of a ref-counted chunk.
 * The chunk gets freed when the ref-count reaches zero.
 */
static int r3_mem_release_shared(void *p);
/**
 * initialize the buffer using given prototype.
 */
static void r3_buffer_init(r3_buffer_t **buffer, r3_buffer_prototype_t *prototype);
/**
 *
 */
void r3_buffer__do_free(r3_buffer_t *buffer);
/**
 * disposes of the buffer
 */
static void r3_buffer_dispose(r3_buffer_t **buffer);
/**
 * allocates a buffer.
 * @param inbuf - pointer to a pointer pointing to the structure (set *inbuf to NULL to allocate a new buffer)
 * @param min_guarantee minimum number of bytes to reserve
 * @return buffer to which the next data should be stored
 * @note When called against a new buffer, the function returns a buffer twice the size of requested guarantee.  The function uses
 * exponential backoff for already-allocated buffers.
 */
r3_iovec_t r3_buffer_reserve(r3_buffer_t **inbuf, unsigned int min_guarantee);
/**
 * throws away given size of the data from the buffer.
 * @param delta number of octets to be drained from the buffer
 */
void r3_buffer_consume(r3_buffer_t **inbuf, unsigned int delta);
/**
 * resets the buffer prototype
 */
static void r3_buffer_set_prototype(r3_buffer_t **buffer, r3_buffer_prototype_t *prototype);
/**
 * registers a buffer to memory pool, so that it would be freed when the pool is flushed.  Note that the buffer cannot be resized
 * after it is linked.
 */
static void r3_buffer_link_to_pool(r3_buffer_t *buffer, r3_mem_pool_t *pool);
void r3_buffer__dispose_linked(void *p);
/**
 * grows the vector so that it could store at least new_capacity elements of given size (or dies if impossible).
 * @param pool memory pool that the vector is using
 * @param vector the vector
 * @param element_size size of the elements stored in the vector
 * @param new_capacity the capacity of the buffer after the function returns
 */
#define r3_vector_reserve(pool, vector, new_capacity)                                                                             \
    r3_vector__reserve((pool), (r3_vector_t *)(void *)(vector), sizeof((vector)->entries[0]), (new_capacity))
static void r3_vector__reserve(r3_mem_pool_t *pool, r3_vector_t *vector, unsigned int element_size, unsigned int new_capacity);
void r3_vector__expand(r3_mem_pool_t *pool, r3_vector_t *vector, unsigned int element_size, unsigned int new_capacity);

/**
 * tests if target chunk (target_len bytes long) is equal to test chunk (test_len bytes long)
 */
static int r3_memis(const void *target, unsigned int target_len, const void *test, unsigned int test_len);

/**
 * secure memset
 */
static void *r3_mem_set_secure(void *b, int c, unsigned int len);

/**
 * swaps contents of memory
 */
void r3_mem_swap(void *x, void *y, unsigned int len);

/**
 * emits hexdump of given buffer to fp
 */
void r3_dump_memory(FILE *fp, const char *buf, unsigned int len);

/**
 * appends an element to a NULL-terminated list allocated using malloc
 */
void r3_append_to_null_terminated_list(void ***list, void *element);

/* inline defs */

inline r3_iovec_t r3_iovec_init(const void *base, unsigned int len)
{
    /* intentionally declared to take a "const void*" since it may contain any type of data and since _some_ buffers are constant */
    r3_iovec_t buf;
    buf.base = (char *)base;
    buf.len = len;
    return buf;
}

inline void *r3_mem_alloc(unsigned int sz)
{
    void *p = malloc(sz);
    if (p == NULL)
        r3_fatal("no memory");
    return p;
}

inline void *r3_mem_realloc(void *oldp, unsigned int sz)
{
    void *newp = realloc(oldp, sz);
    if (newp == NULL) {
        r3_fatal("no memory");
        return oldp;
    }
    return newp;
}

inline void r3_mem_addref_shared(void *p)
{
    struct st_r3_mem_pool_shared_entry_t *entry = R3_STRUCT_FROM_MEMBER(struct st_r3_mem_pool_shared_entry_t, bytes, p);
    assert(entry->refcnt != 0);
    ++entry->refcnt;
}

inline int r3_mem_release_shared(void *p)
{
    struct st_r3_mem_pool_shared_entry_t *entry = R3_STRUCT_FROM_MEMBER(struct st_r3_mem_pool_shared_entry_t, bytes, p);
    if (--entry->refcnt == 0) {
        if (entry->dispose != NULL)
            entry->dispose(entry->bytes);
        free(entry);
        return 1;
    }
    return 0;
}

inline void r3_buffer_init(r3_buffer_t **buffer, r3_buffer_prototype_t *prototype)
{
    *buffer = &prototype->_initial_buf;
}

inline void r3_buffer_dispose(r3_buffer_t **_buffer)
{
    r3_buffer_t *buffer = *_buffer;
    *_buffer = NULL;
    if (buffer->bytes != NULL)
        r3_buffer__do_free(buffer);
}

inline void r3_buffer_set_prototype(r3_buffer_t **buffer, r3_buffer_prototype_t *prototype)
{
    if ((*buffer)->_prototype != NULL)
        (*buffer)->_prototype = prototype;
    else
        *buffer = &prototype->_initial_buf;
}

inline void r3_buffer_link_to_pool(r3_buffer_t *buffer, r3_mem_pool_t *pool)
{
    r3_buffer_t **slot = (r3_buffer_t **)r3_mem_alloc_shared(pool, sizeof(*slot), r3_buffer__dispose_linked);
    *slot = buffer;
}

inline void r3_vector__reserve(r3_mem_pool_t *pool, r3_vector_t *vector, unsigned int element_size, unsigned int new_capacity)
{
    if (vector->capacity < new_capacity) {
        r3_vector__expand(pool, vector, element_size, new_capacity);
    }
}

inline int r3_memis(const void *_target, unsigned int target_len, const void *_test, unsigned int test_len)
{
    const char *target = (const char *)_target, *test = (const char *)_test;
    if (target_len != test_len)
        return 0;
    if (target_len == 0)
        return 1;
    if (target[0] != test[0])
        return 0;
    return memcmp(target + 1, test + 1, test_len - 1) == 0;
}

inline void *r3_mem_set_secure(void *b, int c, unsigned int len)
{
    return r3_mem__set_secure(b, c, len);
}

#ifdef __cplusplus
}
#endif

#endif
