#!/usr/bin/env ruby
arr    = ["foo", "bar", "baz", "qux", "quux", "corge", "grault", "garply"]
paths  = arr.permutation(3).map { |a| "/#{a.join '/'}" }
paths.each do |path|
    puts "r3_tree_insert_path(n, \"#{path}\", NULL, NULL);"
end
