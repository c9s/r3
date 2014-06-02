#!/usr/bin/env ruby

puts <<END

/** DO NOT MODIFY THIS FILE, THIS TEST FILE IS AUTO-GENERATED. **/

#include "config.h"
#include <stdio.h>
#include <check.h>
#include <stdlib.h>
#include <assert.h>
#include "r3.h"
#include "r3_str.h"
#include "zmalloc.h"

START_TEST (test_routes)
{
    node * n = r3_tree_create(10);
    node * m = NULL;

END


arr    = ["foo", "bar", "baz", "qux", "quux", "corge", "grault", "garply"]
paths  = arr.permutation(3).map { |a| "/#{a.join '/'}" }
paths.each_index do |idx|
    path = paths.fetch(idx)
    puts "    char *data#{idx} = \"#{path}\";"
    puts "    r3_tree_insert_path(n, \"#{path}\", (void*) data#{idx});"
end


    puts <<END
    char *err = NULL;
    r3_tree_compile(n, &err);
    ck_assert(err == NULL);
END

paths.each_index do |idx|
    path = paths.fetch(idx)
    puts "    m = r3_tree_match(n, \"#{path}\", NULL);"
    puts "    ck_assert(m != NULL);"
    puts "    ck_assert(m->data == data#{idx});"
    puts "    ck_assert(m->endpoint > 0);"
end



puts <<END

    r3_tree_free(n);

}
END_TEST



Suite* r3_suite (void) {
    Suite *suite = suite_create("r3 routes tests");
    TCase *tcase = tcase_create("testcase");
    tcase_add_test(tcase, test_routes);
    suite_add_tcase(suite, tcase);
    return suite;
}

int main (int argc, char *argv[]) {
    int number_failed;
    Suite *suite = r3_suite();
    SRunner *runner = srunner_create(suite);
    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return number_failed;
}

END
