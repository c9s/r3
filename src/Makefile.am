AM_CFLAGS=$(DEPS_CFLAGS) $(GVC_DEPS_CFLAGS) $(JSONC_CFLAGS) -I$(top_builddir) -I$(top_builddir)/include -I$(top_builddir)/3rdparty  -Wall -std=c99
AM_LDFLAGS=$(DEPS_LIBS) $(GVC_DEPS_LIBS) $(JSONC_LIBS)
MAYBE_COVERAGE=--coverage

noinst_LTLIBRARIES = libr3core.la
# lib_LIBRARIES = libr3.a
libr3core_la_SOURCES = node.c edge.c str.c token.c match_entry.c slug.c

if ENABLE_JSON
libr3core_la_SOURCES += json.c
endif

if COND_GCOV
# MAYBE_COVERAGE=--coverage --no-inline
AM_CFLAGS += $(MAYBE_COVERAGE)
endif

MOSTLYCLEANFILES = *.gcov *.gcda *.gcno



# libr3_la_LDFLAGS = -export-symbols-regex '^r3_|^match_'

# libr3_la_LIBADD=$(DEPS_LIBS) $(LIBOBJS) $(ALLOCA)
# libr3core_la_LIBADD=$(DEPS_LIBS)
# libr3core_la_CFLAGS=$(DEPS_CFLAGS) -I$(top_builddir) -I$(top_builddir)/include -I$(top_builddir)/3rdparty  -Wall -std=c99

if ENABLE_GRAPHVIZ
libr3core_la_SOURCES += gvc.c
endif

# AM_CFLAGS=$(DEPS_CFLAGS)
