### 0.9999 - Mon May 19 10:03:41 2014

API changes:

1. Remove the `route` argument from `r3_tree_insert_pathl`:

    node * r3_tree_insert_pathl(node *tree, char *path, int path_len, void * data);

This reduce the interface complexity, e.g.,

    r3_tree_insert_path(n, "/user2/{id:\\d+}", &var2);

2. The original `r3_tree_insert_pathl` has been renamed to `_r3_tree_insert_path` as a private API.
    

