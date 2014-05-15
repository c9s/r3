/*
 * str.h
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */
#ifndef STR_H
#define STR_H

char * ltrim_slash(char* str);

char** str_split(char* a_str, const char a_delim);

#endif /* !STR_H */
