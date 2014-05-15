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
#include "str.h"
#include "token.h"

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
 * @param char * sep separator
 */
char * slug_to_pcre(char * slug, char sep)
{
    char * p = NULL;
    char * pat = NULL;
    char * end = NULL;

    if ( NULL != (p = strchr(slug, ':')) ) {
        // this slug contains a pattern
        end = strchr(p, '}');

        // start after ':'
        return strndup( (p + 1) , (end - p - 1) );
    } else {
        if ((pat = malloc(128)) == NULL) {
            return (NULL);
        }
        // should return a '[^/]+' pattern
        snprintf(pat, 128, "[^%c]+", sep);
    }
    return pat;
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
