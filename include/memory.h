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

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
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

/**
 * buffer structure compatible with iovec
 */
typedef struct st_r3_iovec_t {
    const char *base;
    unsigned int len;
} r3_iovec_t;

#define R3_VECTOR(type)                                                 \
    struct {                                                            \
        type *entries;                                                  \
        unsigned int size;                                              \
        unsigned int capacity;                                          \
    }

typedef R3_VECTOR(void) r3_vector_t;

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
 * wrapper of realloc; reallocs the given chunk or dies if impossible
 */
static void *r3_mem_realloc(void *oldp, unsigned int sz);

/**
 * grows the vector so that it could store at least new_capacity elements of given size (or dies if impossible).
 * @param vector the vector
 * @param element_size size of the elements stored in the vector
 * @param new_capacity the capacity of the buffer after the function returns
 */
#define r3_vector_reserve(vector, new_capacity)                                                                             \
    r3_vector__reserve((r3_vector_t *)(void *)(vector), sizeof((vector)->entries[0]), (new_capacity))
static void r3_vector__reserve(r3_vector_t *vector, unsigned int element_size, unsigned int new_capacity);
void r3_vector__expand(r3_vector_t *vector, unsigned int element_size, unsigned int new_capacity);

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

inline void r3_vector__reserve(r3_vector_t *vector, unsigned int element_size, unsigned int new_capacity)
{
    if (vector->capacity < new_capacity) {
        r3_vector__expand(vector, element_size, new_capacity);
    }
}

#ifdef __cplusplus
}
#endif

#endif
