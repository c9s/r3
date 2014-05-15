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


/**
 * This function is used to split route path into a string array, not for performance.
 * hence this function should be safe.
 *
 * Split "/path/foo/{id}" into [ "/path" , "/foo" , "/{id}" ]
 * Split "/path/bar/{id}" into [ "/path" , "/foo" , "/{id}" ]
 * Split "/blog/post/{id}" into [ "/blog" , "/post" , "/{id}" ]
 * Split "/blog/{id}" into [ "/blog" , "/{id}" ]
 * Split "/blog" into [ "/blog" ]
 * Split "/b" into [ "/b" ]
 *
 * @param char* pattern
 * @param int   pattern_len
 *
 * @return char**
 */
char** split_route_pattern(char *pattern, int pattern_len) {

    char *p = pattern;

    while (*p) {
        // a slug
        if ( *p == '{' ) {
            char *s1 = p;
            char *s2;
            while (*(p++) != '}') {
                if ( p - pattern > pattern_len ) {
                    // XXX: unexpected error (unclosed slug)
                }
            }
            s2 = p;
        }
    }


    return NULL;
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
