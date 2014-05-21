# INCLUDES                    = @CHECK_CFLAGS@
# if HAVE_CHECK
# TESTS                       = check_tree
# else
# TESTS                       =
# endif
TESTS                       = check_tree
AM_CFLAGS = -ggdb $(DEPS_CFLAGS) -I$(top_builddir) -I$(top_builddir)/include  @CHECK_CFLAGS@
AM_LDFLAGS = $(DEPS_LIBS) -L$(top_builddir)/src -lr3 @CHECK_LIBS@


noinst_HEADERS = \
	bench.h \
	$(NULL)

dist_noinst_DATA = \
	bench_str.csv \
	$(NULL)

if USE_JEMALLOC
AM_CFLAGS += -ljemalloc
endif

if ENABLE_GRAPHVIZ
TESTS += check_gvc
check_gvc_SOURCES = check_gvc.c bench.c
# check_gvc_LDADD = $(GVC_DEPS_LIBS) $(DEPS_LIBS) -L$(top_builddir)/src -lr3 @CHECK_LIBS@
# check_gvc_CFLAGS = $(GVC_DEPS_CFLAGS) $(DEPS_CFLAGS) -I$(top_builddir) -I$(top_builddir)/include @CHECK_CFLAGS@
AM_CFLAGS += $(GVC_DEPS_CFLAGS)
AM_LDFLAGS += $(GVC_DEPS_LIBS)
endif



# noinst_PROGRAMS             = $(TESTS)
check_tree_SOURCES = check_tree.c bench.c
# check_tree_LDADD=$(DEPS_LIBS) -L$(top_builddir)/src -lr3 @CHECK_LIBS@
# check_tree_CFLAGS=$(DEPS_CFLAGS) -I$(top_builddir) -I$(top_builddir)/include @CHECK_CFLAGS@

TESTS += check_slug
check_slug_SOURCES = check_slug.c


check_PROGRAMS = $(TESTS)


# AM_CFLAGS=$(DEPS_CFLAGS) -I$(top_builddir)/include
# AM_CFLAGS=$(DEPS_CFLAGS) -I$(top_builddir) -I$(top_builddir)/include
CLEANFILES                  = check_tree.log
