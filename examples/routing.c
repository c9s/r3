/*
 * check_slug.c
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include <stdio.h>
#include <stdlib.h>
#include "../include/r3.h"




void test1(void) {
    R3Node *n = r3_tree_create(10);

    int route_data = 3;

    // insert the R3Route path into the router tree
    r3_tree_insert_routel(n, METHOD_GET | METHOD_POST, "/blog/{post}/asdi/{id}", sizeof("/blog/{post}/asdi/{id}") - 1, &route_data );
    r3_tree_insert_routel(n, METHOD_GET | METHOD_POST, "/blog/{post}/asf/{id}", sizeof("/blog/{post}/asf/{id}") - 1, &route_data );
    r3_tree_insert_routel(n, METHOD_GET | METHOD_POST, "/blog/{post}/asd/{id:[0-9]+}/qwe", sizeof("/blog/{post}/asd/{id:[0-9]+}/qwe") - 1, &route_data );

    char *errstr = NULL;
    int err = r3_tree_compile(n, &errstr);
    if (err != 0) {
        // fail
        printf("error: %s\n", errstr);
        free(errstr); // errstr is created from `asprintf`, so you have to free it manually.
    }


    // in your http server handler

    // create the match entry for capturing dynamic variables.
    match_entry * entry;
    R3Route *matched_route;
    // for (int k = 0; k < 3000000; k++) {
        entry = match_entry_create("/blog/nnn/asdi/kkkk");
        if (entry != NULL) {
            entry->request_method = METHOD_GET;
            matched_route = r3_tree_match_route(n, entry);
            if (matched_route != NULL) {
                // printf("Routed data is: %d\n", *(int*)matched_route->data); // get the data from matched route
                for (int i = 0; i < entry->vars->len; i++) {
                    // entry->vars->slugs[i];
                    // entry->vars->tokens[i];
                    printf("Slug name is: %s\n",entry->vars->slugs[i]);
                    printf("Slug value is: %s\n",entry->vars->tokens[i]);
                }
            }
        }
        // free the objects at the end
        match_entry_free(entry);
    // }
        entry = match_entry_create("/blog/aaa/asd/123/qwe");
        if (entry != NULL) {
            entry->request_method = METHOD_GET;
            matched_route = r3_tree_match_route(n, entry);
            if (matched_route != NULL) {
                // printf("Routed data is: %d\n", *(int*)matched_route->data); // get the data from matched route
                for (int i = 0; i < entry->vars->len; i++) {
                    // entry->vars->slugs[i];
                    // entry->vars->tokens[i];
                    printf("Slug name is: %s\n",entry->vars->slugs[i]);
                    printf("Slug value is: %s\n",entry->vars->tokens[i]);
                }
            }
        }
        // free the objects at the end
        match_entry_free(entry);
    
    r3_tree_free(n);
}

int main (int argc, char *argv[]) {
    test1();
}
