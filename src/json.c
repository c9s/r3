/*
 * json.c
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include "config.h"
#include <json-c/json.h>
#include "r3.h"
#include "r3_json.h"

json_object * r3_route_to_json_object(const route * r) {
    json_object *obj;

    obj = json_object_new_object();
    json_object_object_add(obj, "path", json_object_new_string(r->path));
    json_object_object_add(obj, "allowed_methods", json_object_new_int(r->request_method));


    if (r->host) {
        json_object_object_add(obj, "host", json_object_new_string(r->host));
    }
    if (r->remote_addr_pattern) {
        json_object_object_add(obj, "remote_addr_pattern", json_object_new_string(r->remote_addr_pattern));
    }
    return obj;
}


json_object * r3_edge_to_json_object(const edge * e) {
    json_object *obj;

    obj = json_object_new_object();
    json_object_object_add(obj, "pattern", json_object_new_string(e->pattern));
    json_object_object_add(obj, "opcode", json_object_new_int(e->opcode));
    json_object_object_add(obj, "slug", json_object_new_boolean(e->has_slug));

    if (e->child) {
        json_object *node_obj = r3_node_to_json_object(e->child);
        json_object_object_add(obj, "child", node_obj);
    }
    return obj;
}

json_object * r3_node_to_json_object(const node * n) {
    json_object *obj;

    obj = json_object_new_object();

    if (n->combined_pattern) {
        json_object_object_add(obj, "re", json_object_new_string(n->combined_pattern));
    }
    json_object_object_add(obj, "endpoint", json_object_new_int(n->endpoint));
    json_object_object_add(obj, "compare", json_object_new_int(n->compare_type));


    int i;

    if ( n->edge_len > 0 ) {
        json_object *edges_array = json_object_new_array();
        json_object_object_add(obj, "edges", edges_array);
        for (i = 0 ; i < n->edge_len ; i++ ) {
            json_object *edge_json_obj = r3_edge_to_json_object(n->edges[i]);
            json_object_array_add(edges_array, edge_json_obj);
        }
    }
    
    if ( n->route_len > 0 ) {
        json_object *routes_array = json_object_new_array();
        json_object_object_add(obj, "routes", routes_array);
        for (i = 0; i < n->route_len; i++ ) {
            json_object *route_json_obj = r3_route_to_json_object(n->routes[i]);
            json_object_array_add(routes_array, route_json_obj);
        }
    }


    return obj;
}


const char * r3_node_to_json_string_ext(const node * n, int options) {
    json_object *obj = r3_node_to_json_object(n);
    return json_object_to_json_string_ext(obj, options);
}

const char * r3_node_to_json_pretty_string(const node * n) {
    json_object *obj = r3_node_to_json_object(n);
    return json_object_to_json_string_ext(obj, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_SPACED);
}

const char * r3_node_to_json_string(const node * n) {
    json_object *obj = r3_node_to_json_object(n);
    return json_object_to_json_string(obj);
}


