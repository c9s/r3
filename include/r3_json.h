/*
 * r3_json.h
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef R3_JSON_H
#define R3_JSON_H

#include <json-c/json.h>
#include "r3.h"

json_object * r3_edge_to_json_object(const edge * e);
json_object * r3_node_to_json_object(const node * n);
json_object * r3_route_to_json_object(const route * r);

const char * r3_node_to_json_string_ext(const node * n, int options);
const char * r3_node_to_json_pretty_string(const node * n);
const char * r3_node_to_json_string(const node * n);



#endif /* !R3_JSON_H */
