R2
================

libr2 is a DFA URI router library. It compiles your route paths into DFA state
table, and you may dispatch the URL based on the transition table in C.



Pattern Syntax
-----------------------

    /blog/post/{id}      use [^/]+ regular expression by default.

    /blog/post/{id:\d+}  use `\d+` regular expression instead of default.



Use case in PHP
-----------------------

```php
// Here is the paths data structure
$paths = [
    '/blog/post/{id}' => [ 'controller' => 'PostController', 'action' => 'item', 'method' => 'GET' ],
    '/blog/post' => [ 'controller' => 'PostController', 'action' => 'list', 'method' => 'GET' ],
    '/blog/post' => [ 'controller' => 'PostController', 'action' => 'create', 'method' => 'POST' ],
    '/blog' => [ 'controller' => 'BlogController', 'action' => 'list', 'method' => 'GET' ],
];
$rs = r2_compile($paths, 'persisten-table-id');
$ret = r2_dispatch($rs, '/blog/post/3' );
list($complete, $route, $variables) = $ret;

list($error, $message) = r2_validate($route); // validate route conditions
if ( $error ) {
    echo $message; // "Method not allowed", "...";
}
```

