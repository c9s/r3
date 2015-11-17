/*
 * r3_str.h
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#ifndef R3_STR_H
#define R3_STR_H

#ifdef __cplusplus
extern "C" {
#endif

char * r3_slug_compile(const char * str, int len);

char * r3_slug_find_pattern(const char *s1, int *len);

char * r3_slug_find_name(const char *s1, int *len);

char * r3_slug_find_placeholder(const char *s1, int *len);

int r3_slug_count(const char * needle, int len, char **errstr);

char * r3_inside_slug(const char * needle, int needle_len, char *offset, char **errstr);

void print_indent(int level);

#ifdef __cplusplus
}
#endif

#endif /* !R3_STR_H */
