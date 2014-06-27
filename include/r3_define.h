/*
 * r3_define.h
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef DEFINE_H
#define DEFINE_H
#include <stdbool.h>

#ifndef bool
typedef unsigned char bool;
#endif
#ifndef FALSE
#    define FALSE 0
#endif
#ifndef TRUE
#    define TRUE 1
#endif

// #define DEBUG 1
#ifdef DEBUG

#define info(fmt, ...) \
            do { fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#define debug(fmt, ...) \
        do { fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)

#else
#define info(...);
#define debug(...);
#endif

#endif /* !DEFINE_H */
