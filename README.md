R3
================

[![Build Status](https://travis-ci.org/c9s/r3.svg?branch=master)](https://travis-ci.org/c9s/r3)

[![Coverage Status](https://coveralls.io/repos/c9s/r3/badge.png)](https://coveralls.io/r/c9s/r3)

R3 is an URL router library with high performance, thus, it's implemented in C.
It compiles your route paths into a prefix trie.

By using the prefix tree constructed in the start-up time, you can dispatch
the path to the controller with high efficiency.



Requirement
-----------------------

### Build Requirement

* autoconf
* automake
* check
* pkg-config

### Runtime Requirement

* pcre
* (optional) graphviz version 2.38.0 (20140413.2041)
* (optional) libjson-c-dev

Pattern Syntax
-----------------------

    /blog/post/{id}      use [^/]+ regular expression by default.
    /blog/post/{id:\d+}  use `\d+` regular expression instead of default.


API
------------------------

```c
#include <r3/r3.h>

// create a router tree with 10 children capacity (this capacity can grow dynamically)
n = r3_tree_create(10);

int route_data = 3;

// insert the route path into the router tree
r3_tree_insert_path(n, "/bar", &route_data); // ignore the length of path

r3_tree_insert_pathl(n, "/zoo", strlen("/zoo"), &route_data );
r3_tree_insert_pathl(n, "/foo/bar", strlen("/foo/bar"), &route_data );

r3_tree_insert_pathl(n ,"/post/{id}", strlen("/post/{id}") , &route_data );

r3_tree_insert_pathl(n, "/user/{id:\\d+}", strlen("/user/{id:\\d+}"), &route_data );


// if you want to catch error, you may call the extended path function for insertion
int data = 10;
char *errstr = NULL;
node *ret = r3_tree_insert_pathl_ex(n, "/foo/{name:\\d{5}", strlen("/foo/{name:\\d{5}"), NULL, &data, &errstr);
if (ret == NULL) {
    // failed insertion
    printf("error: %s\n", errstr);
    free(errstr); // errstr is created from `asprintf`, so you have to free it manually.
}


// let's compile the tree!
char *errstr = NULL;
int err = r3_tree_compile(n, &errstr);
if (err != 0) {
    // fail
    printf("error: %s\n", errstr);
    free(errstr); // errstr is created from `asprintf`, so you have to free it manually.
}


// dump the compiled tree
r3_tree_dump(n, 0);

// match a route
node *matched_node = r3_tree_matchl(n, "/foo/bar", strlen("/foo/bar"), NULL);
if (matched_node) {
    int ret = *( (*int) matched_node->data );
}

// release the tree
r3_tree_free(n);
```


**Capture Dynamic Variables**

If you want to capture the variables from regular expression, you will need to
create a `match_entry` object and pass the object to `r3_tree_matchl` function,
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




**Release Memroy**

To release the memory, you may call `r3_tree_free(node *tree)` to release the whole tree structure, 
`node*`, `edge*`, `route*` objects that were inserted into the tree will be freed.






### Routing with conditions

```c
// create a router tree with 10 children capacity (this capacity can grow dynamically)
n = r3_tree_create(10);

int route_data = 3;

// insert the route path into the router tree
r3_tree_insert_routel(n, METHOD_GET | METHOD_POST, "/blog/post", sizeof("/blog/post") - 1, &route_data );

char *errstr = NULL;
int err = r3_tree_compile(n, &errstr);
if (err != 0) {
    // fail
    printf("error: %s\n", errstr);
    free(errstr); // errstr is created from `asprintf`, so you have to free it manually.
}


// in your http server handler

// create the match entry for capturing dynamic variables.
match_entry * entry = match_entry_create("/blog/post");
entry->request_method = METHOD_GET;


route *matched_route = r3_tree_match_route(n, entry);
matched_route->data; // get the data from matched route

// free the objects at the end
match_entry_free(entry);
r3_tree_free(n);
```

Slug
-----------------------
A slug is a placeholder, which captures the string from the URL as a variable.
Slugs will be compiled into regular expression patterns.

Slugs without patterns (like `/user/{userId}`) will be compiled into the `[^/]+` pattern.

To specify the pattern of a slug, you may write a colon to separate the slug name and the pattern:

    "/user/{userId:\\d+}"

The above route will use `\d+` as its pattern.


Optimization
-----------------------
Simple regular expressions are optimized through a regexp pattern to opcode
translator, which translates simple patterns into small & fast scanners.

By using this method, r3 reduces the matching overhead of pcre library.

Optimized patterns are: `[a-z]+`, `[0-9]+`, `\d+`, `\w+`, `[^/]+` or `[^-]+`

Slugs without specified regular expression will be compiled into the `[^/]+` pattern. therefore, it's optimized too.

Complex regular expressions will still use libpcre to match URL (partially).


Performance
-----------------------
The routing benchmark from stevegraham/rails' PR <https://github.com/stevegraham/rails/pull/1>:

                 omg    10462.0 (±6.7%) i/s -      52417 in   5.030416s

And here is the result of the router journey:

                 omg     9932.9 (±4.8%) i/s -      49873 in   5.033452s

r3 uses the same route path data for benchmarking, and here is the benchmark:

                3 runs, 5000000 iterations each run, finished in 1.308894 seconds
                11460057.83 i/sec


### The Route Paths Of Benchmark

The route path generator is from <https://github.com/stevegraham/rails/pull/1>:

```ruby
#!/usr/bin/env ruby
arr    = ["foo", "bar", "baz", "qux", "quux", "corge", "grault", "garply"]
paths  = arr.permutation(3).map { |a| "/#{a.join '/'}" }
paths.each do |path|
    puts "r3_tree_insert_path(n, \"#{path}\", NULL);"
end
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




Rendering Routes With Graphviz
---------------------------------------

The `r3_tree_render_file` API let you render the whole route trie into a image.

To use graphviz, you need to enable graphviz while you run `configure`:


    ./configure --enable-graphviz


Here is the sample code of generating graph output:


```c
node * n = r3_tree_create(1);

r3_tree_insert_path(n, "/foo/bar/baz",  NULL);
r3_tree_insert_path(n, "/foo/bar/qux",  NULL);
r3_tree_insert_path(n, "/foo/bar/quux",  NULL);
r3_tree_insert_path(n, "/foo/bar/corge",  NULL);
r3_tree_insert_path(n, "/foo/bar/grault",  NULL);
r3_tree_insert_path(n, "/garply/grault/foo",  NULL);
r3_tree_insert_path(n, "/garply/grault/bar",  NULL);
r3_tree_insert_path(n, "/user/{id}",  NULL);
r3_tree_insert_path(n, "/post/{title:\\w+}",  NULL);

char *errstr = NULL;
int err;
err = r3_tree_compile(n, &errstr);
if (err != 0) {
    // fail
    printf("error: %s\n", errstr);
    free(errstr); // errstr is created from `asprintf`, so you have to free it manually.
}

r3_tree_render_file(n, "png", "check_gvc.png");
r3_tree_free(n);
```


![Imgur](http://imgur.com/HrUoEbI.png)

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

### Graphviz Related Functions

```c
int r3_tree_render_file(const node * tree, const char * format, const char * filename);

int r3_tree_render(const node * tree, const char *layout, const char * format, FILE *fp);

int r3_tree_render_dot(const node * tree, const char *layout, FILE *fp);

int r3_tree_render_file(const node * tree, const char * format, const char * filename);
```


JSON Output
----------------------------------------

You can render the whole tree structure into json format output.

Please run `configure` with the `--enable-json` option.

Here is the sample code to generate JSON string:

```c
json_object * obj = r3_node_to_json_object(n);

const char *json = r3_node_to_json_pretty_string(n);
printf("Pretty JSON: %s\n",json);

const char *json = r3_node_to_json_string(n);
printf("JSON: %s\n",json);
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
    sudo make install

And we support debian-based distro now!

    sudo apt-get install build-essential autoconf automake libpcre3-dev pkg-config debhelper libtool check
    mv dist-debian debian
    dpkg-buildpackage -b -us -uc
    sudo gdebi ../libr3*.deb


#### Run Unit Tests

    ./configure --enable-check
    make check

#### Enable Graphviz

    ./configure --enable-graphviz

#### With jemalloc

    ./configure --with-malloc=jemalloc

ubuntu PPA
----------------------

The PPA for libr3 can be found in <https://launchpad.net/~r3-team/+archive/libr3-daily>.

Binding For Other Languages
---------------------------

* Perl Router::R3 by @CindyLinz <https://metacpan.org/pod/Router::R3>
* Python pyr3 by @lucemia <https://github.com/lucemia/pyr3>
* Python pyr3 by @thedrow <https://github.com/thedrow/pyr3>
* Haskell r3 by @MnO2 <https://github.com/MnO2/r3>
* Vala r3-vala by @Ronmi <https://github.com/Ronmi/r3-vala>
* Node.js node-r3 by @othree <https://github.com/othree/node-r3>
* Node.js node-libr3 by @caasi <https://github.com/caasi/node-r3>


License
--------------------
This software is released under MIT License.
