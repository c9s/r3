/*
 * slug.c
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "r3.h"
#include "r3_str.h"
#include "slug.h"
#include "zmalloc.h"

r3_slug_t * r3_slug_new(char * path, int path_len) {
    r3_slug_t * s = zmalloc(sizeof(r3_slug_t));
    s->path = path;
    s->path_len = path_len;

    s->begin = NULL;
    s->end = NULL;
    s->len = 0;

    s->pattern = NULL;
    s->pattern_len = 0;
    return s;
}

void r3_slug_free(r3_slug_t * s) {
    zfree(s);
}


/**
 * Return 1 means OK
 * Return 0 means Empty
 * Return -1 means Error
 */
int r3_slug_check(r3_slug_t *s, char **errstr) {
    // if it's empty
    if (s->begin == NULL && s->len == 0) {
        return 0;
    }
    if (s->begin && s->begin == s->end && s->len == 0) {
        return 0;
    }

    // if the head is defined, we should also have end pointer
    if (s->begin && s->end == NULL) {
        return -1;
    }
    return 0;
}


char * r3_slug_to_str(r3_slug_t *s) {
    char *str = NULL;
    asprintf(&str, "slug: '%.*s', pattern: '%.*s', path: '%.*s'", s->len, s->begin, s->pattern_len, s->pattern, s->path_len, s->path);
    return str;
}


/*
r3_slug_t * r3_slug_parse_next(r3_slug_t *s, char **errstr) {
    return r3_slug_parse(s->end, s->path_len - (s->end - s->begin), errstr);
}
*/

r3_slug_t * r3_slug_parse(const char *needle, int needle_len, char *offset, char **errstr) {
    r3_slug_t * s = r3_slug_new(needle, needle_len);

    int cnt = 0;
    int state = 0;
    char * p = offset;

    while( (p-needle) < needle_len) {

        if (*p == '\\' ) {
            p++; p++;
        }
        if (state == 0 && *p == '{') {
            s->begin = p+1; 
        }

        if (state == 1 && *p == ':') {
            // start from next
            s->pattern = p+1;
        }

        // closing slug
        if (state == 1 && *p == '}') {
            s->end = p;
            s->len = s->end - s->begin;
            if (s->pattern) {
                s->pattern_len = p - s->pattern;
            }

            cnt++;
            state--;
            break;
        }

        if ( *p == '{' ) {
            state++;
        } else if ( *p == '}' ) {
            state--;
        }
        p++;
    };

    if (state > 0) {
        if (errstr) {
            asprintf(errstr, "incomplete slug pattern. PATH (%d): '%s', OFFSET: %ld, STATE: %d", needle_len, needle, p - needle, state);
        }
        return NULL;
    }

    return s;
}
