lib_LTLIBRARIES = libr3.la
# lib_LIBRARIES = libr3.a
libr3_la_SOURCES = node.c edge.c str.c token.c zmalloc.c
# libr3_la_LDFLAGS = -export-symbols-regex '^r3_|^match_'

libr3_la_LIBADD=$(DEPS_LIBS)
AM_CFLAGS=$(DEPS_CFLAGS) -I$(top_builddir) -I$(top_builddir)/include -Wall -std=c99

if USE_JEMALLOC
AM_CFLAGS += -ljemalloc
endif

if ENABLE_DEBUG
AM_CFLAGS += -ggdb -fprofile-arcs -ftest-coverage
endif

if ENABLE_GRAPHVIZ
libr3_la_SOURCES += gvc.c
libr3_la_LIBADD += $(GVC_DEPS_LIBS)
AM_CFLAGS += $(GVC_DEPS_CFLAGS)
endif

# AM_CFLAGS=$(DEPS_CFLAGS)
