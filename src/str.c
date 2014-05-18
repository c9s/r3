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

int strndiff(char * d1, char * d2, unsigned int n) {
    char * o = d1;
    while ( *d1 == *d2 && n-- > 0 ) { 
        d1++;
        d2++;
    }
    return d1 - o;
}


int strdiff(char * d1, char * d2) {
    char * o = d1;
    while( *d1 == *d2 ) { 
        d1++;
        d2++;
    }
    return d1 - o;
}


/**
 * provide a quick way to count slugs, simply search for '{'
 */
int count_slug(char * p, int len) {
    int s = 0;
    while( len-- ) {
        if ( *p == '{' )
            s++;
        p++;
    }
    return s;
}

bool contains_slug(char * str) {
    return strchr(str, '{') != NULL ? TRUE : FALSE;
}

/**
 * @param char * sep separator
 */
char * compile_slug(char * str, int len)
{
    char *s1 = NULL, *s2 = NULL, *o = NULL;
    char *pat = NULL;
    char sep = '/';

    // find '{'
    s1 = strchr(str, '{');

    if ( s1 == NULL ) {
        return strdup(str);
    }

    if ( (s1 - str) > 0 ) {
        sep = *(s1-1);
    }

    char * out = NULL;
    if ((out = calloc(sizeof(char),128)) == NULL) {
        return (NULL);
    }

    // append prefix
    o = out;
    strncat(o, str, s1 - str);
    o += (s1 - str);

    // start after ':'
    if ( NULL != (pat = strchr(s1, ':')) ) {
        pat++;

        // this slug contains a pattern
        s2 = strchr(pat, '}');

        *o = '(';
        o++;

        strncat(o, pat, (s2 - pat) );
        o += (s2 - pat);

        *o = ')';
        o++;

    } else {
        // should return a '[^/]+' pattern
        // strncat(c, "([^%c]+)", strlen("([^%c]+)") );
        // snprintf(pat, 128, "([^%c]+)", sep);
        sprintf(o, "([^%c]+)", sep);
        o+= sizeof("([^%c]+)");
    }

    s2++;
    while( (s2 - str) > len ) {
        *o = *s2;
        s2++;
        o++;
    }
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
