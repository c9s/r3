bin_PROGRAMS=test_tree
test_tree_SOURCES=test_tree.c
test_tree_LDADD=$(DEPS_LIBS) -L$(top_builddir)/src -lcheck -lr3
# AM_CFLAGS=$(DEPS_CFLAGS) -I$(top_builddir)/include
AM_CFLAGS=$(DEPS_CFLAGS) -I$(top_builddir) -I$(top_builddir)/include
