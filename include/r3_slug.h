/*
 * r3_str.h
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#ifndef R3_SLUG_H
#define R3_SLUG_H

#ifdef __cplusplus
extern "C" {
#endif

char * r3_slug_compile(const char * str, unsigned int len);

const char * r3_slug_find_pattern(const char *s1, unsigned int str_len, unsigned int *len);

const char * r3_slug_find_name(const char *s1, unsigned int str_len, unsigned int *len);

const char * r3_slug_find_placeholder(const char *s1, unsigned int str_len, unsigned int *len);

int r3_slug_count(const char * needle, int len, char **errstr);

char * r3_inside_slug(const char * needle, int needle_len, char *offset, char **errstr);

#ifdef __cplusplus
}
#endif

#endif /* !R3_SLUG_H */
