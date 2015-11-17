/*
 * r3_define.h
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef DEFINE_H
#define DEFINE_H
#include <stdbool.h>
#if !defined(bool) && !defined(__cplusplus)
typedef unsigned char bool;
#endif
#ifndef FALSE
#    define FALSE 0
#endif
#ifndef TRUE
#    define TRUE 1
#endif
#endif /* !DEFINE_H */
