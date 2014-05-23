R3
================

[![Build Status](https://travis-ci.org/c9s/r3.svg?branch=master)](https://travis-ci.org/c9s/r3)

[![Coverage Status](https://coveralls.io/repos/c9s/r3/badge.png)](https://coveralls.io/r/c9s/r3)

R3 is an URL router library with high performance, thus, it's implemented in C.
It compiles your route paths into a prefix trie.

By using the constructed prefix trie in the start-up time, you can dispatch
routes with efficiency.


Requirement
-----------------------

* autoconf
* automake
* check
* pcre
* graphviz version 2.38.0 (20140413.2041)
* pkg-config

Pattern Syntax
-----------------------

    /blog/post/{id}      use [^/]+ regular expression by default.
    /blog/post/{id:\d+}  use `\d+` regular expression instead of default.


C API
------------------------

```c
#include <r3.h>

// create a router tree with 10 children capacity (this capacity can grow dynamically)
n = r3_tree_create(10);

int route_data = 3;

// insert the route path into the router tree
r3_tree_insert_path(n, "/bar", &route_data); // ignore the length of path

r3_tree_insert_pathl(n, "/zoo", strlen("/zoo"), &route_data );
r3_tree_insert_pathl(n, "/foo/bar", strlen("/foo/bar"), &route_data );

r3_tree_insert_pathl(n ,"/post/{id}", strlen("/post/{id}") , &route_data );

r3_tree_insert_pathl(n, "/user/{id:\\d+}", strlen("/user/{id:\\d+}"), &route_data );

// let's compile the tree!
r3_tree_compile(n);


// dump the compiled tree
r3_tree_dump(n, 0);

// match a route
node *matched_node = r3_tree_match(n, "/foo/bar", strlen("/foo/bar"), NULL);
if (matched_node) {
    matched_node->endpoint; // make sure there is a route end at here.
    int ret = *( (*int) matched_node->route_ptr );
}

// release the tree
r3_tree_free(n);
```

If you want to capture the variables from regular expression, you will need to create a match entry,
the catched variables will be pushed into the match entry structure:

```c
match_entry * entry = match_entry_create("/foo/bar");

// free the match entry
match_entry_free(entry);
```

And you can even specify the request method restriction:

```c
entry->request_method = METHOD_GET;
entry->request_method = METHOD_POST;
entry->request_method = METHOD_GET | METHOD_POST;
```

When using `match_entry`, you may match the route with `r3_tree_match_entry` function:

```c
node *matched_node = r3_tree_match_entry(n, entry);
```

### Routing with conditions

```c
// create the match entry for capturing dynamic variables.
match_entry * entry = match_entry_create("/foo/bar");
entry->request_method = METHOD_GET;

// create a router tree with 10 children capacity (this capacity can grow dynamically)
n = r3_tree_create(10);

int route_data = 3;

// define the route with conditions
route *r1 = route_create("/blog/post");
r1->request_method = METHOD_GET | METHOD_POST; // ALLOW GET OR POST METHOD

// insert the route path into the router tree
r3_tree_insert_route(n, r1, &route_data );
r3_tree_compile(n);

route *matched_route = r3_tree_match_route(n, entry);
matched_route->data; // get the data from matched route

// free the objects at the end
r3_route_free(r1);
r3_tree_free(n);
```


Function prefix mapping
-----------------------

|Function Prefix   |Description                                                                         |
|------------------|------------------------------------------------------------------------------------|
|`r3_tree_*`       |Tree related operations, which require a node to operate a whole tree               |
|`r3_node_*`       |Single node related operations, which do not go through its own children or parent. |
|`r3_edge_*`       |Edge related operations                                                             |
|`r3_route_*`      |Route related operations, which are needed only when the tree is defined by routes  |
|`match_entry_*`   |Match entry related operations, a `match_entry` is just like the request parameters |


Performance
-----------------------
The routing benchmark from stevegraham/rails' PR <https://github.com/stevegraham/rails/pull/1>:

                 omg    10462.0 (±6.7%) i/s -      52417 in   5.030416s

And here is the result of the router journey:

                 omg     9932.9 (±4.8%) i/s -      49873 in   5.033452s

r3 uses the same route path data for benchmarking, and here is the benchmark:

                3 runs, 5000000 iterations each run, finished in 1.308894 seconds
                11460057.83 i/sec


### The benchmarking route paths 

The route path generator is from <https://github.com/stevegraham/rails/pull/1>:

```ruby
#!/usr/bin/env ruby
arr    = ["foo", "bar", "baz", "qux", "quux", "corge", "grault", "garply"]
paths  = arr.permutation(3).map { |a| "/#{a.join '/'}" }
paths.each do |path|
    puts "r3_tree_insert_path(n, \"#{path}\", NULL);"
end
```


Rendering routes with graphviz
-------------------------------

The `test_gvc_render_file` API let you render the whole route trie into a image.

![Imgur](http://i.imgur.com/J2LdzeK.png)

Or you can even export it with dot format:

```dot
digraph g {
	graph [bb="0,0,205.1,471"];
	node [label="\N"];
	"{root}"	 [height=0.5,
		pos="35.097,453",
		width=0.97491];
	"#1"	 [height=0.5,
		pos="35.097,366",
		width=0.75];
        ....
```

Use case in PHP
-----------------------
**not implemented yet**

```php
// Here is the paths data structure
$paths = [
    '/blog/post/{id}' => [ 'controller' => 'PostController' , 'action' => 'item'   , 'method'   => 'GET' ] , 
    '/blog/post'      => [ 'controller' => 'PostController' , 'action' => 'list'   , 'method'   => 'GET' ] , 
    '/blog/post'      => [ 'controller' => 'PostController' , 'action' => 'create' , 'method' => 'POST' ]  , 
    '/blog'           => [ 'controller' => 'BlogController' , 'action' => 'list'   , 'method'   => 'GET' ] , 
];
$rs = r3_compile($paths, 'persisten-table-id');
$ret = r3_dispatch($rs, '/blog/post/3' );
list($complete, $route, $variables) = $ret;

// matched conditions aren't done yet
list($error, $message) = r3_validate($route); // validate route conditions
if ( $error ) {
    echo $message; // "Method not allowed", "...";
}
```

Install
----------------------

    sudo apt-get install check libpcre3 libpcre3-dev libjemalloc-dev libjemalloc1 build-essential libtool automake autoconf pkg-config
    sudo apt-get install graphviz-dev graphviz  # if you want graphviz
    ./autogen.sh
    ./configure && make
    make check # run tests
    sudo make install

#### Enable Graphviz

    ./configure --enable-graphviz

#### With jemalloc

    ./configure --with-malloc=jemalloc


License
--------------------
This software is released under MIT License.
