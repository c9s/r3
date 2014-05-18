R3
================

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
* jemalloc
* graphviz version 2.38.0 (20140413.2041)

Pattern Syntax
-----------------------

    /blog/post/{id}      use [^/]+ regular expression by default.
    /blog/post/{id:\d+}  use `\d+` regular expression instead of default.


C API
------------------------

```c
// create a router tree with 10 children capacity (this capacity can grow dynamically)
n = r3_tree_create(10);

int route_data = 3;

// insert the route path into the router tree
r3_tree_insert_pathn(n , "/zoo"       , strlen("/zoo")       , &route_data );
r3_tree_insert_pathn(n , "/foo/bar"   , strlen("/foo/bar")   , &route_data );
r3_tree_insert_pathn(n , "/bar"       , strlen("/bar")       , &route_data );
r3_tree_insert_pathn(n , "/post/{id}" , strlen("/post/{id}") , &route_data );

// let's compile the tree!
r3_tree_compile(n);


// dump the compiled tree
r3_tree_dump(n, 0);

// match a route
node *matched_node = r3_tree_match(n, "/foo/bar", strlen("/foo/bar") );
matched_node->endpoint; // make sure there is a route end at here.
int ret = *( (*int) matched_node->route_ptr );
```



Benchmark
-----------------------
The routing benchmark from stevegraham/rails' PR <https://github.com/stevegraham/rails/pull/1>:

                 omg    10462.0 (±6.7%) i/s -      52417 in   5.030416s

And here is the result of the router journey:

                 omg     9932.9 (±4.8%) i/s -      49873 in   5.033452s

r3 uses the same route path data for benchmarking, and here is the benchmark:

                3 runs, 5000000 iterations each run, finished in 1.308894 seconds
                11460057.83 i/sec

The matching speed of r3 is 1153+ times faster than rails' trie router.


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

    sudo apt-get install check libpcre3 libpcre3-dev libjemalloc-dev libjemalloc1 build-essential libtool automake autoconf graphviz-dev graphviz
    ./autogen.sh
    ./configure && make
    make check # run tests
    sudo make install

### Enable Graphviz

    ./configure --enable-graphviz


