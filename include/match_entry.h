/*
 * match_entry.h
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef MATCH_ENTRY_H
#define MATCH_ENTRY_H

#include "r3.h"

match_entry * match_entry_createl(const char * path, int path_len);

#define match_entry_create(path) match_entry_createl(path,strlen(path))

void match_entry_free(match_entry * entry);

#endif /* !MATCH_ENTRY_H */
