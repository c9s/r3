/*
 * str.c
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "r3.h"
#include "r3_str.h"
#include "str_array.h"
#include "zmalloc.h"

int r3_pattern_to_opcode(char * pattern, int pattern_len) {
    if ( strncmp(pattern, "\\w+", pattern_len) == 0 ) {
        return OP_EXPECT_WORDS;
    }
    if ( strncmp(pattern, "\\d+", pattern_len) == 0 ) {
        return OP_EXPECT_DIGITS;
    }
    if ( strncmp(pattern, "[^/]+", pattern_len) == 0 ) {
        return OP_EXPECT_NOSLASH;
    }
    return 0;
}



/**
 * provide a quick way to count slugs, simply search for '{'
 */
int slug_count(char * p, int len) {
    int s = 0;
    int lev = 0;
    while( len-- ) {
        if ( lev == 0 && *p == '{' )
            s++;
        if ( *p == '{' ) {
            lev++;
        }
        if ( *p == '}' ) {
            lev--;
        }
        p++;
    }
    return s;
}

bool contains_slug(char * str) {
    return strchr(str, '{') != NULL ? TRUE : FALSE;
}

char * inside_slug(char * needle, int needle_len, char *offset) {
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

    char * end = needle + needle_len;
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
    return NULL;
}

char * find_slug_placeholder(char *s1, int *len) {
    char *c;
    char *s2;
    int cnt = 0;
    if ( NULL != (c = strchr(s1, '{')) ) {
        // find closing '}'
        s2 = c;
        while(*s2) {
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
char * find_slug_pattern(char *s1, int *len) {
    char *c;
    char *s2;
    int cnt = 1;
    if ( NULL != (c = strchr(s1, ':')) ) {
        c++;
        // find closing '}'
        s2 = c;
        while(s2) {
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
    *len = s2 - c;
    return c;
}


/**
 * @param char * sep separator
 */
char * slug_compile(char * str, int len)
{
    char *s1 = NULL, *o = NULL;
    char *pat = NULL;
    char sep = '/';


    // append prefix
    int s1_len;
    s1 = find_slug_placeholder(str, &s1_len);

    if ( s1 == NULL ) {
        return zstrdup(str);
    }

    char * out = NULL;
    if ((out = zcalloc(200)) == NULL) {
        return (NULL);
    }

    o = out;
    strncat(o, str, s1 - str); // string before slug
    o += (s1 - str);


    int pat_len;
    pat = find_slug_pattern(s1, &pat_len);

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
    strncat(o, s1, strlen(s1));
    return out;
}


char * ltrim_slash(char* str)
{
    char * p = str;
    while (*p == '/') p++;
    return zstrdup(p);
}

void str_repeat(char *s, char *c, int len) {
    while(len--) {
        s[len - 1] = *c;
    }
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
