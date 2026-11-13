# CHANGELOG

by Yo-An Lin <yoanlin93@gmail.com>


### 2.0.0 - Tue Sep 29 2026

Breaking changes:

- Renamed the `node`, `edge` and `route` structs to `R3Node`, `R3Edge` and
  `R3Route`, and added an `R3` prefix to the related public types.
- Changed several public function signatures beyond the type renames, including
  `r3_tree_insert_pathl_ex`, `r3_node_append_edge` and `r3_node_append_route`.
- Reworked the `str_array` API and the `match_entry` path field, and replaced
  `r3_edge_createl` with `r3_edge_initl`.
- Removed the obsolete `r3_route_create` and `r3_route_createl` declarations.
  Create routes with `r3_node_append_route` instead.
- Migrated from PCRE to PCRE2. R3 now links against the PCRE2 library
  (pkg-config module `libpcre2-8`) instead of the original PCRE. Install your
  platform's PCRE2 development package (for example `libpcre2-dev` on
  Debian/Ubuntu).

Other changes:

- Removed the bundled `3rdparty/zmalloc` allocator. Memory helpers now live in
  `include/memory.h` and `src/memory.c`.
- Reorganized public headers: removed `r3_define.h` and `r3_str.h`; added
  `r3_slug.h`, which exposes the slug helper functions.
- Modernized the CMake build: replaced `FindPCRE.cmake` with
  `FindPCRE2.cmake`, moved the finders under `cmake/Modules/`, and fixed the
  header install path.
- Moved continuous integration from Travis CI to GitHub Actions, and added a
  Coverity scan workflow.

New features:

- Match against the HTTP scheme.
- Match against the host, including wildcard host matching.
- Match against the remote IP address, including IPv6 addresses.
- Optimized the `.*` pattern so it can be used for prefix matching.

Security and robustness fixes:

- Fixed a buffer overflow in `r3_slug_compile`.
- Fixed a buffer overflow in `r3_tree_compile_patterns`.
- Fixed an out-of-bounds read in `r3_node_find_common_prefix`.
- Fixed buffer over-read errors in route matching.
- Corrected router match logic and handling of nodes with multiple edges.
- Fixed a memory leak in `r3_tree_insert_pathl_ex` and leaks in the test suite.
- Resolved Clang and Coverity warnings.


### 1.3.4  - Tue Nov 17 18:27:25 2015

- Faster edge branching.
- Fixed zero-length path insertion.
- Fixed a memory leak and freed slug objects correctly.
- C++ compatibility: added `extern "C"` guards and stopped typedef'ing `bool`
  when compiling as C++.
- Added `r3_slug_find_name` and renamed `slug_count` to `r3_slug_count`.
- Build fixes for CMake and pkg-config.


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


