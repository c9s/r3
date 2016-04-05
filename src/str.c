/*
 * str.c
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "r3.h"
#include "r3_slug.h"
#include "str.h"
#include "slug.h"
#include "zmalloc.h"

static char * strnchr(const char* str, unsigned int len, int ch) {
    for (unsigned int i = 0; i < len; i++) {
        if (str[i] == ch) return str + i;
    }
    return NULL;
}

int r3_pattern_to_opcode(const char * pattern, unsigned int len) {
    if ( strncmp(pattern, "\\w+",len) == 0 ) {
        return OP_EXPECT_MORE_WORDS;
    }
    if ( strncmp(pattern, "[0-9a-z]+",len) == 0 ||  strncmp(pattern, "[a-z0-9]+",len) == 0  ) {
        return OP_EXPECT_MORE_WORDS;
    }
    if ( strncmp(pattern, "[a-z]+",len) == 0 ) {
        return OP_EXPECT_MORE_ALPHA;
    }
    if ( strncmp(pattern, "\\d+", len) == 0 ) {
        return OP_EXPECT_MORE_DIGITS;
    }
    if ( strncmp(pattern, "[0-9]+", len) == 0 ) {
        return OP_EXPECT_MORE_DIGITS;
    }
    if ( strncmp(pattern, "[^/]+", len) == 0 ) {
        return OP_EXPECT_NOSLASH;
    }
    if ( strncmp(pattern, "[^-]+", len) == 0 ) {
        return OP_EXPECT_NODASH;
    }
    return 0;
}




char * r3_inside_slug(const char * needle, int needle_len, char *offset, char **errstr) {
    char * s1 = offset;
    char * s2 = offset;

    short found_s1 = 0;
    short found_s2 = 0;

    while( s1 >= needle && (s1 - needle < needle_len) ) {
        if ( *s1 == '{' ) {
            found_s1 = 1;
            break;
        }
        s1--;
    }

    const char * end = needle + needle_len;
    while( (s2 + 1) < end ) {
        if ( *s2 == '}' ) {
            found_s2 = 1;
            break;
        }
        s2++;
    }
    if (found_s1 && found_s2) {
        return s1;
    }
    if (found_s1 || found_s2) {
        // wrong slug pattern
        if(errstr) {
            asprintf(errstr, "Incomplete slug pattern");
        }
        return NULL;
    }
    return NULL;
}

char * r3_slug_find_placeholder(const char *s1, unsigned int str_len, unsigned int *len) {
    char *c;
    char *s2;
    int cnt = 0;
    if (c = strnchr(s1, str_len, '{')) {
        // find closing '}'
        s2 = c;
        unsigned int j = str_len - (c - s1);
        for (unsigned int i = 0; i < j; i++) {
            if (*s2 == '{' )
                cnt++;
            else if (*s2 == '}' )
                cnt--;
            if (cnt == 0)
                break;
            s2++;
        }
    } else {
        return NULL;
    }
    if (cnt!=0) {
        return NULL;
    }
    if(len) {
        *len = s2 - c + 1;
    }
    return c;
}


/**
 * given a slug string, duplicate the pattern string of the slug
 */
char * r3_slug_find_pattern(const char *s1, unsigned int str_len, unsigned int *len) {
    char *c;
    char *s2;
    unsigned int cnt = 1;
    if ( (c = strnchr(s1, str_len, ':')) ) {
        c++;
        // find closing '}'
        s2 = c;
        unsigned int j = str_len - (c - s1);
        for (unsigned int i = 0; i < j; i++) {
            if (*s2 == '{' )
                cnt++;
            else if (*s2 == '}' )
                cnt--;
            if (cnt == 0)
                break;
            s2++;
        }

    } else {
        return NULL;
    }
    if (cnt!=0) {
        return NULL;
    }
    *len = s2 - c;
    return c;
}


/**
 * given a slug string, duplicate the parameter name string of the slug
 */
char * r3_slug_find_name(const char *s1, unsigned int str_len, unsigned int *len) {
    char * c;
    char * s2;
    int cnt = 0;
    unsigned int plholder;
    if (c = r3_slug_find_placeholder(s1, str_len, &plholder)) {
        c++;
        if ( s2 = strnchr(c, plholder, ':') ) {
            *len = s2 - c;
            return c;
        } else {
            *len = plholder - 2;
            return c;
        }
    } else {
        return NULL;
    }
}


/**
 * @param char * sep separator
 */
char * r3_slug_compile(const char * str, unsigned int len)
{
    char *s1 = NULL, *o = NULL;
    char *pat = NULL;
    char sep = '/';


    // append prefix
    unsigned int s1_len;
    s1 = r3_slug_find_placeholder(str, len, &s1_len);

    if ( !s1 ) {
        return zstrndup(str,len);
    }

    char * out = NULL;
    if (!(out = zcalloc(sizeof(char) * 200))) {
        return (NULL);
    }

    o = out;
    strncat(o, "^", 1);
    o++;

    strncat(o, str, s1 - str); // string before slug
    o += (s1 - str);


    unsigned int pat_len;
    pat = r3_slug_find_pattern(s1, s1_len, &pat_len);

    if (pat) {
        *o = '(';
        o++;
        strncat(o, pat, pat_len );
        o += pat_len;
        *o = ')';
        o++;
    } else {
        sprintf(o, "([^%c]+)", sep);
        o+= strlen("([^*]+)");
    }
    s1 += s1_len;
    strncat(o, s1, len - (s1 - str)); // string after slug
    return out;
}


char * ltrim_slash(char* str)
{
    char * p = str;
    while (*p == '/') p++;
    return zstrdup(p);
}

void print_indent(int level) {
    int len = level * 2;
    while(len--) {
        printf(" ");
    }
}



#ifndef HAVE_STRDUP
char *zstrdup(const char *s) {
    char *out;
    int count = 0;
    while( s[count] )
        ++count;
    ++count;
    out = zmalloc(sizeof(char) * count);
    out[--count] = 0;
    while( --count >= 0 )
        out[count] = s[count];
    return out;
}
#endif



#ifndef HAVE_STRNDUP
char *zstrndup(const char *s, int n) {
    char *out;
    int count = 0;
    while( count < n && s[count] )
        ++count;
    ++count;
    out = zmalloc(sizeof(char) * count);
    out[--count] = 0;
    while( --count >= 0 )
        out[count] = s[count];
    return out;
}
#endif
