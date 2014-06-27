# CHANGELOG

by Yo-An Lin <yoanlin93@gmail.com>


### 1.3.3  - Sat Jun 28 00:53:48 2014

- Fix graphviz generator.


### 1.3.2  - Sat Jun 28 00:54:22 2014

- `HAVE_STRNDUP` and `HAVE_STRDUP` definition fix

### 1.3.0  - Tue Jun  3 18:47:14 2014

- Added Incorrect slug syntax warnings
- Added error message support for pcre/pcre-jit compile
- Added JSON encode support for the tree structure
- Improved Graphivz Related Functions
- More failing test cases

### 1.2.1  - Tue May 27 21:16:13 2014

- Bug fixes.
- Function declaration improvement.
- pkg-config flags update (r3.pc)

### 1.2    - Fri May 23 23:30:11 2014

- Added simple pattern optimization.
- Clean up.
- Bug fixes.

### 0.9999 - Mon May 19 10:03:41 2014

API changes:

1. Removed the `route` argument from `r3_tree_insert_pathl_ex`:

        node * r3_tree_insert_pathl_ex(node *tree, char *path, int path_len, void * data);

    This reduce the interface complexity, e.g.,

        r3_tree_insert_path(n, "/user2/{id:\\d+}", &var2);

2. The original `r3_tree_insert_pathl_ex` has been moved to `r3_tree_insert_pathl_ex` as a private API.

3. Moved `r3_tree_matchl` to `r3_tree_matchl` since it require the length of the path string.

        m = r3_tree_matchl( n , "/foo", strlen("/foo"), entry);

4. Added `r3_tree_match` for users to match a path without the length of the path string.

        m = r3_tree_match( n , "/foo", entry);

5. Added `r3_tree_match_entry` for users want to match a `match_entry`, which is just a macro to simplify the use:


        #define r3_tree_match_entry(n, entry) r3_tree_matchl(n, entry->path, entry->path_len, entry)


6. Please note that A path that is inserted by `r3_tree_insert_route` can only be matched by `r3_tree_match_route`.

7. Added `r3_` prefix to `route` related methods.


