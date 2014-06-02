TESTS =

AM_CFLAGS=$(DEPS_CFLAGS) $(GVC_DEPS_CFLAGS) $(JSONC_CFLAGS) @CHECK_CFLAGS@ -I$(top_builddir) -I$(top_builddir)/include -I$(top_builddir)/src -I$(top_builddir)/3rdparty  -Wall -std=c99 -ggdb -Wall
AM_LDFLAGS=$(DEPS_LIBS) $(GVC_DEPS_LIBS) $(JSONC_LIBS) @CHECK_LIBS@ $(top_builddir)/libr3.la

if USE_JEMALLOC
AM_CFLAGS += -ljemalloc
endif

noinst_HEADERS = \
	bench.h \
	$(NULL)

dist_noinst_DATA = \
	$(NULL)


TESTS += check_slug
check_slug_SOURCES = check_slug.c

TESTS += check_tree
check_tree_SOURCES = check_tree.c

TESTS += check_routes
check_routes_SOURCES = check_routes.c

TESTS += check_str_array
check_str_array_SOURCES = check_str_array.c


if ENABLE_JSON
TESTS += check_json
check_json_SOURCES = check_json.c
endif


if ENABLE_GRAPHVIZ
TESTS += check_gvc
check_gvc_SOURCES = check_gvc.c
endif



check_PROGRAMS = $(TESTS)

noinst_PROGRAMS = bench
bench_SOURCES = bench.c

# AM_CFLAGS=$(DEPS_CFLAGS) -I$(top_builddir)/include
# AM_CFLAGS=$(DEPS_CFLAGS) -I$(top_builddir) -I$(top_builddir)/include
CLEANFILES                  = check_tree.log

