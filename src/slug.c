/*
 * slug.c
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "r3.h"
#include "r3_str.h"
#include "slug.h"
#include "zmalloc.h"



r3_slug_t * r3_slug_new(char * path, int path_len) {
    r3_slug_t * s = zmalloc(sizeof(r3_slug_t));
    if (!s)
        return NULL;
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
int r3_slug_check(r3_slug_t *s) {
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


char * r3_slug_to_str(const r3_slug_t *s) {
    char *str = NULL;
    asprintf(&str, "slug: '%.*s', pattern: '%.*s', path: '%.*s'", s->len, s->begin, s->pattern_len, s->pattern, s->path_len, s->path);
    return str;
}



/*
r3_slug_t * r3_slug_parse_next(r3_slug_t *s, char **errstr) {
    return r3_slug_parse(s->end, s->path_len - (s->end - s->begin), errstr);
}
*/

r3_slug_t * r3_slug_parse(char *needle, int needle_len, char *offset, char **errstr) {
    r3_slug_t * s = r3_slug_new(needle, needle_len);
    if (!s) {
        return NULL;
    }

    if (!offset) {
        offset = (char*) needle; // from the begining of the needle
    }

    // there is no slug
    if (!r3_path_contains_slug_char(offset)) {
        return NULL;
    }

    int cnt = 0;
    int state = 0;
    char * p = offset;

    while( (p-needle) < needle_len) {

        // escape one character
        if (*p == '\\' ) {
            p++; p++;
            continue;
        }

        // slug starts with '{'
        if (state == 0 && *p == '{') {
            s->begin = ++p;
            state++;
            continue;
        }

        // in the middle of the slug (pattern)
        if (state == 1 && *p == ':') {
            // start from next
            s->pattern = ++p;
            continue;
        }

        // slug closed.
        if (state == 1 && *p == '}') {
            s->end = p;
            s->len = s->end - s->begin;
            if (s->pattern) {
                s->pattern_len = p - s->pattern;
            }
            cnt++;
            state--;
            p++;
            break;
        }

        // might be inside the pattern
        if ( *p == '{' ) {
            state++;
        } else if ( *p == '}' ) {
            state--;
        }
        p++;
    };

    if (state > 0) {
        if (errstr) {
            asprintf(errstr, "Incomplete slug pattern. PATH (%d): '%s', OFFSET: %ld, STATE: %d", needle_len, needle, p - needle, state);
        }
        return NULL;
    }
    return s;
}


/**
 * provide a quick way to count slugs, simply search for '{'
 */
int slug_count(const char * needle, int len, char **errstr) {
    int cnt = 0;
    int state = 0;
    char * p = (char*) needle;

    while( (p-needle) < len) {

        if (*p == '\\' ) {
            p++; p++;
        }

        if (state == 1 && *p == '}') {
            cnt++;
        }
        if ( *p == '{' ) {
            state++;
        } else if ( *p == '}' ) {
            state--;
        }
        p++;
    };
    info("FOUND PATTERN: '%s' (%d), STATE: %d\n", needle, len, state);
    if (state != 0) {
        if (errstr) {
            asprintf(errstr, "Incomplete slug pattern. PATTERN (%d): '%s', OFFSET: %ld, STATE: %d", len, needle, p - needle, state);
        }
        return 0;
    }
    return cnt;
}


