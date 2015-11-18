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

json_object * r3_edge_to_json_object(const R3Edge * e);
json_object * r3_node_to_json_object(const R3Node * n);
json_object * r3_route_to_json_object(const R3Route * r);

const char * r3_node_to_json_string_ext(const R3Node * n, int options);
const char * r3_node_to_json_pretty_string(const R3Node * n);
const char * r3_node_to_json_string(const R3Node * n);



#endif /* !R3_JSON_H */
