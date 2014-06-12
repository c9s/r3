#include <cstdlib>
#include <cstring>
#include <iostream>
#include <r3.hpp>

using namespace std;

void example_1() {
    // create a router tree with 10 children capacity (this capacity can grow dynamically)
    r3::Tree tree(10);

    // insert the route path into the router tree
    int route_data_1 = 1;
    tree.insert_path("/bar", &route_data_1); // ignore the length of path

    int route_data_2 = 2;
    tree.insert_pathl("/zoo", strlen("/zoo"), &route_data_2);
    int route_data_3 = 3;
    tree.insert_pathl("/foo/bar", strlen("/foo/bar"), &route_data_3);

    int route_data_4 = 4;
    tree.insert_pathl("/post/{id}", strlen("/post/{id}") , &route_data_4);

    int route_data_5 = 5;
    tree.insert_pathl("/user/{id:\\d+}", strlen("/user/{id:\\d+}"),
        &route_data_5);

    // if you want to catch error, you may call the extended path function for insertion
    int data = 10;
    char* errstr;
    r3::Node ret = tree.insert_pathl("/foo/{name:\\d{5}",
        strlen("/foo/{name:\\d{5}"), &data, &errstr);
    if (ret == NULL) {
        // failed insertion
        cout << "error: " << errstr << endl;
        free(errstr); // errstr is created from `asprintf`, so you have to free it manually.
    }

    // let's compile the tree!
    int err = tree.compile(&errstr);
    if (err != 0) {
        cout << "error: " << errstr << endl;
        free(errstr); // errstr is created from `asprintf`, so you have to free it manually.
    }

    // dump the compiled tree
    tree.dump(0);

    // match a route
    r3::Node matched_node = tree.matchl("/foo/bar", strlen("/foo/bar"));
    if (matched_node) {
        int ret = *static_cast<int*>(matched_node.data());
        cout << "match path ret: " << ret << endl;
    }

    r3::MatchEntry entry("/foo/bar");
    matched_node = tree.match_entry(entry);
    if (matched_node) {
        int ret = *static_cast<int*>(matched_node.data());
        cout << "match entry ret: " << ret << endl;
    }
}

void example_2() {
    // create a router tree with 10 children capacity (this capacity can grow dynamically)
    r3::Tree tree(10);

    // insert the route path into the router tree
    int route_data = 1;
    tree.insert_routel(METHOD_GET | METHOD_POST, "/blog/post",
        sizeof("/blog/post") - 1, &route_data);

    char* errstr;
    int err = tree.compile(&errstr);
    if (err != 0) {
        cout << "errstr: " << errstr << endl;
        free(errstr); // errstr is created from `asprintf`, so you have to free it manually.
    }

    // in your http server handler

    // create the match entry for capturing dynamic variables.
    r3::MatchEntry entry("/blog/post");
    entry.set_request_method(METHOD_GET);

    r3::Route matched_route = tree.match_route(entry);
    if (matched_route) {
        int ret = *static_cast<int*>(matched_route.data());
        cout << "match route ret: " << ret << endl;
    }
}

int main() {
    example_1();
    example_2();
    return 0;
}
