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
#include "r3_str.h"
#include "str_array.h"
#include "r3_define.h"

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

    while( s1 >= needle ) {
        if ( *s1 == '{' ) {
            break;
        }
        s1--;
    }

    char *end = needle+ needle_len;
    while( s2 < end ) {
        if ( *s2 == '}' ) {
            break;
        }
        s2++;
    }

    if ( *s1 == '{' && *s2 == '}' ) {
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
        return strdup(str);
    }

    char * out = NULL;
    if ((out = calloc(sizeof(char),200)) == NULL) {
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
    return strdup(p);
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
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
char *strdup(const char *s) {
    char *out;
    int count = 0;
    while( s[count] )
        ++count;
    ++count;
    out = malloc(sizeof(char) * count);
    out[--count] = 0;
    while( --count >= 0 )
        out[count] = s[count];
    return out;
}
#endif

#ifndef HAVE_STRNDUP
char *strndup(const char *s, int n) {
    char *out;
    int count = 0;
    while( count < n && s[count] )
        ++count;
    ++count;
    out = malloc(sizeof(char) * count);
    out[--count] = 0;
    while( --count >= 0 )
        out[count] = s[count];
    return out;
}
#endif
