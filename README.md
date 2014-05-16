R3
================

R3 is an URI router library. It compiles your route paths into a radix tree.
By using the constructed radix tree in the start-up time, you may dispatch your
routes efficiently.

Pattern Syntax
-----------------------

    /blog/post/{id}      use [^/]+ regular expression by default.

    /blog/post/{id:\d+}  use `\d+` regular expression instead of default.


C API
------------------------

```c
// create a router tree with 10 children capacity (this capacity can grow dynamically)
n = rtree_create(10);

int route_data = 3;

// insert the route path into the router tree
rtree_insert_pathn(n , "/zoo"       , strlen("/zoo")       , &route_data );
rtree_insert_pathn(n , "/foo/bar"   , strlen("/foo/bar")   , &route_data );
rtree_insert_pathn(n , "/bar"       , strlen("/bar")       , &route_data );
rtree_insert_pathn(n , "/post/{id}" , strlen("/post/{id}") , &route_data );

// let's compile the tree!
rtree_compile(n);


// dump the compiled tree
rtree_dump(n, 0);

// match a route
node *matched_node = rtree_match(n, "/foo/bar", strlen("/foo/bar") );
matched_node->endpoint; // make sure there is a route end at here.
int ret = *( (*int) matched_node->route_ptr );
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
$rs = r2_compile_radix($paths, 'persisten-table-id');
$ret = r2_dispatch_radix($rs, '/blog/post/3' );
list($complete, $route, $variables) = $ret;

list($error, $message) = r2_validate($route); // validate route conditions
if ( $error ) {
    echo $message; // "Method not allowed", "...";
}
```

